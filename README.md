tccPerimeter — Accurate Digital Contour Perimeter (Vossepoel & Smeulders–based)
OpenCV’s built-in perimeter function cv::arcLength prioritizes speed and relies on polygonal approximation; in high-precision metrology it can produce systematic errors, especially on pixelated, 4/8-connected contours.
TCC Perimeter implements a perimeter estimator derived from Vossepoel & Smeulders. It counts horizontal/vertical and diagonal unit steps along the pixel chain (no polygon simplification), adds a corner correction, and yields a more accurate estimate of the true Euclidean perimeter of digital objects.

Key Idea
Given a per-pixel contour (obtained with CHAIN_APPROX_NONE), we count:
Ne — number of horizontal/vertical unit steps
No — number of diagonal unit steps
X — number of direction changes (“corners”)
Perimeter is estimated as:

P=a⋅Ne+b⋅No−c⋅X

with empirically optimized coefficients (from the original work and common practice):
a = 0.980 (H/V step weight)
b = 1.406 (diagonal step weight)
c = 0.091 (corner correction)
These values reduce bias compared to naive 4- or 8-neighbor step metrics and typical polygonal approximations.

Why not cv::arcLength?

arcLength operates on polygonal curves (often simplified or quantized), which can under/over-estimate length on digital contours and small features.
For high-precision measurement on binary silhouettes or sub-pixel-critical edges, a digital-geometry-aware estimator (like Vossepoel–Smeulders) produces lower bias across orientations and curvatures.
