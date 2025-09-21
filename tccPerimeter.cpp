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
    long X  = 0;  // number of corners (for correction term)

    int prevDir = -1;

    for (size_t i = 0; i < n; ++i) {
        cv::Point p1 = contour[i];
        cv::Point p2 = contour[(i + 1) % n];

        int dx = p2.x - p1.x;
        int dy = p2.y - p1.y;
        if (dx == 0 && dy == 0)
            continue;

        int steps = std::max(std::abs(dx), std::abs(dy));
        int stepx = (dx > 0) ? 1 : (dx < 0 ? -1 : 0);
        int stepy = (dy > 0) ? 1 : (dy < 0 ? -1 : 0);

        for (int k = 0; k < steps; ++k) 
        {
            int d = -1;
            if (stepx == 1 && stepy == 0) d = 0;
            else if (stepx == 1 && stepy == -1) d = 1;
            else if (stepx == 0 && stepy == -1) d = 2;
            else if (stepx == -1 && stepy == -1) d = 3;
            else if (stepx == -1 && stepy == 0) d = 4;
            else if (stepx == -1 && stepy == 1) d = 5;
            else if (stepx == 0 && stepy == 1) d = 6;
            else if (stepx == 1 && stepy == 1) d = 7;

            if (d == -1)
                continue;

            if ((d % 2) == 0) ++Ne; else 
              ++No;

            if (prevDir != -1 && d != prevDir) 
              ++X;
            prevDir = d;
        }
    }

    return a * static_cast<float>(Ne) + b * static_cast<float>(No) - c * static_cast<float>(X);
}
