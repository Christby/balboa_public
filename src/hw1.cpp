#include "hw1.h"
#include "hw1_scenes.h"

using namespace hw1;

Image3 hw_1_1(const std::vector<std::string> &params) {
    // Homework 1.1: render a circle at the specified
    // position, with the specified radius and color.

    Image3 img(640 /* width */, 480 /* height */);

    Vector2 center = Vector2{img.width / 2 + Real(0.5), img.height / 2 + Real(0.5)};
    Real radius = 100.0;
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Vector3 background = Vector3{0.5, 0.5, 0.5};

    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-center") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            center = Vector2{x, y};
        } else if (params[i] == "-radius") {
            radius = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        }
    }

    for (int x = 0; x < img.width; x++) {
        for (int y = 0; y < img.height; y++) {
            Real x_center = x + 0.5;
            Real y_center = y + 0.5;
            Real dist = std::sqrt(std::pow(x_center - center.x, 2) + std::pow(y_center - center.y, 2));

            if (dist <= radius) {
                img(x, y) = color;
            } else {
                img(x, y) = background;
            }
        }
    }

    return img;
}

Image3 hw_1_2(const std::vector<std::string> &params) {
    // Homework 1.2: render multiple circles
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    int scene_id = std::stoi(params[0]);
    const CircleScene &scene = hw1_2_scenes[scene_id];

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }

    for (const Circle &circle : scene.objects) {
        int x_min = std::max(0, int(circle.center.x - circle.radius));
        int x_max = std::min(img.width - 1, int(circle.center.x + circle.radius));
        int y_min = std::max(0, int(circle.center.y - circle.radius));
        int y_max = std::min(img.height - 1, int(circle.center.y + circle.radius));

        for (int x = x_min; x < x_max; x++) {
            for (int y = y_min; y < y_max; y++) {
                Real pixel_center_x = x + 0.5;
                Real pixel_center_y = y + 0.5;
                Real dist = std::sqrt(std::pow(pixel_center_x - circle.center.x, 2) + std::pow(pixel_center_y - circle.center.y, 2));

                if (dist <= circle.radius) {
                    img(x, y) = circle.color;
                }
            }
        }
    }
    return img;
}

// Helper function to sort the triangle vertices following the specified logic.
void process_triangle_vertices(Vector2& p0, Vector2& p1, Vector2& p2) {
    // Find the vertex with the smallest y-coordinate.
    if (p1.y < p0.y) std::swap(p0, p1);
    if (p2.y < p0.y) std::swap(p0, p2);

    // Determine p1 and p2 based on their x-coordinates.
    if (p1.x < p2.x) {
        std::swap(p1, p2); // Ensure p1 has the larger x-coordinate.
    }
}

Image3 hw_1_3(const std::vector<std::string> &params) {
    // Homework 1.3: render multiple shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }

    for (const auto &shape : scene.shapes) {
        if (auto *circle = std::get_if<Circle>(&shape)) {
            // circle
            for (int x = 0; x < img.width; x++) {
                for (int y = 0; y < img.height; y++) {
                    Real x_center = x + 0.5;
                    Real y_center = y + 0.5;
                    Real dist = std::sqrt(std::pow(x_center - circle->center.x, 2) +
                                          std::pow(y_center - circle->center.y, 2));
                    if (dist <= circle->radius) {
                        img(x, y) = circle->color;
                    }
                }
            }
        } else if (auto *rect = std::get_if<Rectangle>(&shape)) {
            // rectangle
            for (int x = std::max(0, int(rect->p_min.x)); x <= std::min(img.width - 1, int(rect->p_max.x)); x++) {
                for (int y = std::max(0, int(rect->p_min.y)); y <= std::min(img.height - 1, int(rect->p_max.y)); y++) {
                    img(x, y) = rect->color;
                }
            }
        } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
            // triangle
            Vector2 p0 = triangle->p0;
            Vector2 p1 = triangle->p1;
            Vector2 p2 = triangle->p2;
            process_triangle_vertices(p0, p1, p2);

            Vector2 n01 = Vector2(p0.y - p1.y, p1.x - p0.x);
            Vector2 n12 = Vector2(p1.y - p2.y, p2.x - p1.x);
            Vector2 n20 = Vector2(p2.y - p0.y, p0.x - p2.x);

            for (int x = 0; x < img.width; x++) {
                for (int y = 0; y < img.height; y++) {
                    Vector2 point{x + 0.5, y + 0.5};
                    Vector2 v0 = point - p0;
                    Vector2 v1 = point - p1;
                    Vector2 v2 = point - p2;

                    if (dot(n01, v0) >= 0 && dot(n12, v1) >= 0 && dot(n20, v2) >= 0) {
                        img(x, y) = triangle->color;
                    }
                }
            }
        }
    }
    return img;
}

Image3 hw_1_4(const std::vector<std::string> &params) {
    // Homework 1.4: render transformed shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }

    for (const auto &shape : scene.shapes) {
        if (auto *circle = std::get_if<Circle>(&shape)) {
            // circle
            Matrix3x3 transform = circle->transform;
            Matrix3x3 inverse_transform = inverse(transform);

            for (int x = 0; x < img.width; x++) {
                for (int y = 0; y < img.height; y++) {
                    Vector3 pixel_center = inverse_transform * Vector3{x + 0.5, y + 0.5, 1.0};
                    Real dist = std::sqrt(
                        std::pow(pixel_center.x - circle->center.x, 2) +
                        std::pow(pixel_center.y - circle->center.y, 2)
                    );
                    if (dist <= circle->radius) {
                        img(x, y) = circle->color;
                    }
                }
            }
        } else if (auto *rect = std::get_if<Rectangle>(&shape)) {
            // rectangle
            Matrix3x3 transform = rect->transform;
            Matrix3x3 inverse_transform = inverse(transform);

            for (int x = 0; x < img.width; x++) {
                for (int y = 0; y < img.height; y++) {
                    Vector3 pixel_center = inverse_transform * Vector3{x + 0.5, y + 0.5, 1.0};
                    if (pixel_center.x >= rect->p_min.x && pixel_center.x <= rect->p_max.x &&
                        pixel_center.y >= rect->p_min.y && pixel_center.y <= rect->p_max.y) {
                        img(x, y) = rect->color;
                    }
                }
            }
        } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
            // triangle
            Matrix3x3 transform = triangle->transform;
            Matrix3x3 inverse_transform = inverse(transform);

            Vector2 p0 = triangle->p0;
            Vector2 p1 = triangle->p1;
            Vector2 p2 = triangle->p2;
            process_triangle_vertices(p0, p1, p2);

            for (int x = 0; x < img.width; x++) {
                for (int y = 0; y < img.height; y++) {
                    Vector3 pixel_center_3d = inverse_transform * Vector3{x + 0.5, y + 0.5, 1.0};
                    Vector2 pixel_center{pixel_center_3d.x, pixel_center_3d.y};
                    Vector2 v0 = pixel_center - p0;
                    Vector2 v1 = pixel_center - p1;
                    Vector2 v2 = pixel_center - p2;
                    Vector2 n01 = Vector2(p0.y - p1.y, p1.x - p0.x);
                    Vector2 n12 = Vector2(p1.y - p2.y, p2.x - p1.x);
                    Vector2 n20 = Vector2(p2.y - p0.y, p0.x - p2.x);

                    if ((n01.x * v0.x + n01.y * v0.y >= 0) &&
                        (n12.x * v1.x + n12.y * v1.y >= 0) &&
                        (n20.x * v2.x + n20.y * v2.y >= 0)) {
                        img(x, y) = triangle->color;
                    }
                }
            }
        }
    }

    return img;
}

Image3 hw_1_5(const std::vector<std::string> &params) {
    // Homework 1.5: antialiasing
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }

    const int num_subpixels = 4;
    const Real weight = 1.0 / (num_subpixels * num_subpixels);

    for (int x = 0; x < img.width; x++) {
        for (int y = 0; y < img.height; y++) {
            Vector3 accumulated_color{0, 0, 0};

            for (int i = 0; i < num_subpixels; i++) {
                for (int j = 0; j < num_subpixels; j++) {
                    Real x_center = x + (i + 0.5) / num_subpixels;
                    Real y_center = y + (j + 0.5) / num_subpixels;

                    Vector3 color = scene.background;

                    for (const auto &shape : scene.shapes) {
                        if (auto *circle = std::get_if<Circle>(&shape)) {
                            Matrix3x3 inverse_transform = inverse(circle->transform);
                            Vector3 pixel_center = inverse_transform * Vector3{x_center, y_center, 1.0};
                            Real dist = std::sqrt(
                                std::pow(pixel_center.x - circle->center.x, 2) +
                                std::pow(pixel_center.y - circle->center.y, 2)
                            );
                            if (dist <= circle->radius) {
                                color = circle->color;
                            }
                        } else if (auto *rect = std::get_if<Rectangle>(&shape)) {
                            Matrix3x3 inverse_transform = inverse(rect->transform);
                            Vector3 pixel_center = inverse_transform * Vector3{x_center, y_center, 1.0};
                            if (pixel_center.x >= rect->p_min.x && pixel_center.x <= rect->p_max.x &&
                                pixel_center.y >= rect->p_min.y && pixel_center.y <= rect->p_max.y) {
                                color = rect->color;
                            }
                        } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
                            Matrix3x3 inverse_transform = inverse(triangle->transform);
                            Vector3 pixel_center_3d = inverse_transform * Vector3{x_center, y_center, 1.0};
                            Vector2 pixel_center{pixel_center_3d.x, pixel_center_3d.y};

                            Vector2 p0 = triangle->p0;
                            Vector2 p1 = triangle->p1;
                            Vector2 p2 = triangle->p2;
                            process_triangle_vertices(p0, p1, p2);

                            Vector2 v0 = pixel_center - p0;
                            Vector2 v1 = pixel_center - p1;
                            Vector2 v2 = pixel_center - p2;
                            Vector2 n01{p0.y - p1.y, p1.x - p0.x};
                            Vector2 n12{p1.y - p2.y, p2.x - p1.x};
                            Vector2 n20{p2.y - p0.y, p0.x - p2.x};

                            if ((n01.x * v0.x + n01.y * v0.y >= 0) &&
                                (n12.x * v1.x + n12.y * v1.y >= 0) &&
                                (n20.x * v2.x + n20.y * v2.y >= 0)) {
                                color = triangle->color;
                            }
                        }
                    }
                    accumulated_color += color * weight;
                }
            }
            img(x, y) = accumulated_color;
        }
    }
    return img;
}



Image3 hw_1_6(const std::vector<std::string> &params) {
    // Homework 1.6: alpha blending
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int x = 0; x < img.width; ++x) {
        for (int y = 0; y < img.height; ++y) {
            Vector3 blended_color = scene.background;
            Real accumulated_alpha = 0.0;

            for (const auto &shape : scene.shapes) {
                Real alpha = 0.0;
                Vector3 color = scene.background;

                if (auto *circle = std::get_if<Circle>(&shape)) {
                    Real dist = std::sqrt(
                        std::pow(x + 0.5 - circle->center.x, 2) +
                        std::pow(y + 0.5 - circle->center.y, 2)
                    );
                    if (dist <= circle->radius) {
                        color = circle->color;
                        alpha = circle->alpha;
                    }
                } else if (auto *rect = std::get_if<Rectangle>(&shape)) {
                    if (x + 0.5 >= rect->p_min.x && x + 0.5 <= rect->p_max.x &&
                        y + 0.5 >= rect->p_min.y && y + 0.5 <= rect->p_max.y) {
                        color = rect->color;
                        alpha = rect->alpha;
                    }
                } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
                    Vector2 p0 = triangle->p0;
                    Vector2 p1 = triangle->p1;
                    Vector2 p2 = triangle->p2;
                    process_triangle_vertices(p0, p1, p2);

                    Vector2 v0{x + 0.5 - p0.x, y + 0.5 - p0.y};
                    Vector2 v1{x + 0.5 - p1.x, y + 0.5 - p1.y};
                    Vector2 v2{x + 0.5 - p2.x, y + 0.5 - p2.y};
                    Vector2 n01{p0.y - p1.y, p1.x - p0.x};
                    Vector2 n12{p1.y - p2.y, p2.x - p1.x};
                    Vector2 n20{p2.y - p0.y, p0.x - p2.x};

                    if ((n01.x * v0.x + n01.y * v0.y >= 0) &&
                        (n12.x * v1.x + n12.y * v1.y >= 0) &&
                        (n20.x * v2.x + n20.y * v2.y >= 0)) {
                        color = triangle->color;
                        alpha = triangle->alpha;
                    }
                }
                blended_color = alpha * color + (1 - alpha) * blended_color;
                accumulated_alpha = alpha + (1 - alpha) * accumulated_alpha;
            }
            img(x, y) = blended_color;
        }
    }
    return img;
}
