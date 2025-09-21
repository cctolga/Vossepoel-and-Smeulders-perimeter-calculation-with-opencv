// main.cpp
#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

// -------- Vossepoel-based perimeter  --------
// Estimates digital contour length with Vossepoel coefficients.
// Expects a per-pixel chain (use CHAIN_APPROX_NONE in findContours).
float tccPerimeter(const std::vector<cv::Point>& contour)
{
    size_t n = contour.size();
    if (n < 2)
        return 0.0f;

    // Vossepoel coefficients
    const float a = 0.980f;  // weight for horizontal/vertical steps
    const float b = 1.406f;  // weight for diagonal steps
    const float c = 0.091f;  // corner correction factor

    long Ne = 0;  // number of horizontal/vertical steps
    long No = 0;  // number of diagonal steps
    long X  = 0;  // number of corners (direction changes)

    int prevDir = -1;

    for (size_t i = 0; i < n; ++i) {
        cv::Point p1 = contour[i];
        cv::Point p2 = contour[(i + 1) % n]; // closed loop

        int dx = p2.x - p1.x;
        int dy = p2.y - p1.y;
        if (dx == 0 && dy == 0)
            continue;

        // Decompose the segment into unit 4/8-connected steps
        int steps = std::max(std::abs(dx), std::abs(dy));
        int stepx = (dx > 0) ? 1 : (dx < 0 ? -1 : 0);
        int stepy = (dy > 0) ? 1 : (dy < 0 ? -1 : 0);

        for (int k = 0; k < steps; ++k) {
            int d = -1; // direction code in {0..7}
            if (stepx == 1 && stepy == 0) d = 0;
            else if (stepx == 1 && stepy == -1) d = 1;
            else if (stepx == 0 && stepy == -1) d = 2;
            else if (stepx == -1 && stepy == -1) d = 3;
            else if (stepx == -1 && stepy == 0) d = 4;
            else if (stepx == -1 && stepy == 1) d = 5;
            else if (stepx == 0 && stepy == 1) d = 6;
            else if (stepx == 1 && stepy == 1) d = 7;

            if (d == -1) continue;

            // Even directions are H/V, odd directions are diagonals.
            if ((d % 2) == 0) ++Ne; else ++No;

            // Count corners: increment when direction changes.
            if (prevDir != -1 && d != prevDir) ++X;
            prevDir = d;
        }
    }

    // Vossepoel perimeter estimation
    return a * static_cast<float>(Ne)
         + b * static_cast<float>(No)
         - c * static_cast<float>(X);
}

int main(int argc, char** argv)
{


    // --- 2) Read image (BGR) ---
    cv::Mat img = cv::imread("image.jpg", cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cerr << "Error: cannot read image: " <<  "\n";
        return 2;
    }

    // --- 3) Convert to grayscale + Gaussian blur ---
    cv::Mat gray, blur;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    cv::GaussianBlur(gray, blur, cv::Size(5,5), 0);

    // --- 4) Threshold (Otsu) to obtain a binary mask ---
    cv::Mat bin;
    cv::threshold(blur, bin, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // --- 5) Find contours with CHAIN_APPROX_NONE (pixel-precise chain) ---
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(bin, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    if (contours.empty()) {
        std::cout << "No contours found.\n";
        return 0;
    }

    // --- 6) Compute Vossepoel perimeter for each contour ---
    double totalPerimeter = 0.0;
    size_t largestIdx = 0;
    double largestPerimeter = -1.0;

    for (size_t i = 0; i < contours.size(); ++i) {
        float p = tccPerimeter(contours[i]);
        totalPerimeter += p;
        if (p > largestPerimeter) 
        { 
            largestPerimeter = p; 
            largestIdx = i; 
        }
        std::cout << "Contour " << i << " perimeter (Vossepoel): " << p << "\n";
    }

    std::cout << "Total perimeter (sum of all contours): " << totalPerimeter << "\n";
    std::cout << "Largest contour index: " << largestIdx<< " perimeter: " << largestPerimeter << "\n";
    return 0;
}
