#include "hw2.h"
#include "hw2_scenes.h"

using namespace hw2;

bool inTriangle(Real px, Real py, Vector2& p0, Vector2& p1, Vector2& p2) {
    Vector2 n01{p0.y - p1.y, p1.x - p0.x};
    Vector2 n12{p1.y - p2.y, p2.x - p1.x};
    Vector2 n20{p2.y - p0.y, p0.x - p2.x};

    Vector2 pixel_center{px, py};
    Vector2 v0 = pixel_center - p0;
    Vector2 v1 = pixel_center - p1;
    Vector2 v2 = pixel_center - p2;
    
    return ((n01.x * v0.x + n01.y * v0.y >= 0) &&
           (n12.x * v1.x + n12.y * v1.y >= 0) &&
           (n20.x * v2.x + n20.y * v2.y >= 0)) || 
           ((n01.x * v0.x + n01.y * v0.y <= 0) &&
           (n12.x * v1.x + n12.y * v1.y <= 0) &&
           (n20.x * v2.x + n20.y * v2.y <= 0));
}

Image3 hw_2_1(const std::vector<std::string> &params) {
    // Homework 2.1: render a single 3D triangle

    Image3 img(640 /* width */, 480 /* height */);

    Vector3 p0{0, 0, -1};
    Vector3 p1{1, 0, -1};
    Vector3 p2{0, 1, -1};
    Real s = 1; // scaling factor of the view frustrum
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Real z_near = 1e-6; // distance of the near clipping plane
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-p0") {
            p0.x = std::stof(params[++i]);
            p0.y = std::stof(params[++i]);
            p0.z = std::stof(params[++i]);
        } else if (params[i] == "-p1") {
            p1.x = std::stof(params[++i]);
            p1.y = std::stof(params[++i]);
            p1.z = std::stof(params[++i]);
        } else if (params[i] == "-p2") {
            p2.x = std::stof(params[++i]);
            p2.y = std::stof(params[++i]);
            p2.z = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        }
    }

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{0.5, 0.5, 0.5};
        }
    }

    // skip if any vertex is behind the camera
    if (p0.z > -z_near || p1.z > -z_near || p2.z > -z_near) {
        return img;
    }

    // project 3D points to 2D
    auto project = [](const Vector3 &p) -> Vector2 {
        return Vector2{-p.x / p.z, -p.y / p.z};
    };
    Vector2 p0_proj = project(p0);
    Vector2 p1_proj = project(p1);
    Vector2 p2_proj = project(p2);

    // camera-screen conversion
    Real alpha = Real(img.width) / img.height;
    auto camera_to_screen = [=](const Vector2& v) -> Vector2 {
        Real x_screen = img.width * (v.x + s * alpha) / (2 * s * alpha);
        Real y_screen = img.height * (s - v.y) / (2 * s);
        return Vector2{x_screen, y_screen};
    };
    Vector2 p0_screen = camera_to_screen(p0_proj);
    Vector2 p1_screen = camera_to_screen(p1_proj);
    Vector2 p2_screen = camera_to_screen(p2_proj);

    // render the triangle
    const int num_subpixels = 4;
    Real weight = 1.0 / (num_subpixels * num_subpixels);
    for (int x = 0; x < img.width; x++) {
        for (int y = 0; y < img.height; y++) {
            Vector3 accumulated_color{0, 0, 0};
            for (int i = 0; i < num_subpixels; i++) {
                for (int j = 0; j < num_subpixels; j++) {
                    Real sub_x = x + (i + 0.5) / num_subpixels;
                    Real sub_y = y + (j + 0.5) / num_subpixels;

                    if (inTriangle(sub_x, sub_y, p0_screen, p1_screen, p2_screen)) {
                        accumulated_color += color;
                    } else {
                        accumulated_color += Vector3{0.5, 0.5, 0.5};
                    }
                }
            }
            img(x, y) = accumulated_color * weight;
        }
    }

    return img;
}

Real area(Vector3 p0, Vector3 p1, Vector3 p2){
    Vector3 cross_product = cross(p1 - p0, p2 - p0);
    return 0.5 * sqrt(cross_product.x * cross_product.x + cross_product.y * cross_product.y + cross_product.z * cross_product.z);
}

Image3 hw_2_2(const std::vector<std::string> &params) {
    // Homework 2.2: render a triangle mesh

    Image3 img(640 /* width */, 480 /* height */);
    Real alpha = Real(img.width) / img.height;

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{0.5, 0.5, 0.5};
        }
    }

    const int num_subpixels = 4;
    Real weight = 1.0 / (num_subpixels * num_subpixels);
    for (int x = 0; x < img.width; x++) {
        for (int y = 0; y < img.height; y++) {
            Vector3 accumulated_color{0, 0, 0};

            for (int i = 0; i < num_subpixels; i++) {
                for (int j = 0; j < num_subpixels; j++) {
                    Real sub_x = x + (i + 0.5) / num_subpixels;
                    Real sub_y = y + (j + 0.5) / num_subpixels;
                    Vector3 color = Vector3{0.5, 0.5, 0.5};

                    Vector3 p_camera((sub_x * 2 * s * alpha)/img.width - s * alpha, (1 - sub_y/img.height) * 2 * s - s, -1.0);
                    Real z_min = -infinity<Real>();

                    for (int idx = 0; idx < mesh.faces.size(); idx++) {
                        const Vector3i& face = mesh.faces[idx];
                        Vector3 p0 = mesh.vertices[face[0]];
                        Vector3 p1 = mesh.vertices[face[1]];
                        Vector3 p2 = mesh.vertices[face[2]];

                        if ((-z_near < p0.z) || (-z_near < p1.z) || (-z_near < p2.z)){
                            continue;
                        }

                        Vector3 p0_camera(-p0.x/p0.z, -p0.y/p0.z, -1.0);
                        Vector3 p1_camera(-p1.x/p1.z, -p1.y/p1.z, -1.0);
                        Vector3 p2_camera(-p2.x/p2.z, -p2.y/p2.z, -1.0);

                        Real triangle_area = area(p0_camera, p1_camera, p2_camera);
                        Real b0_camera = area(p_camera, p1_camera, p2_camera) / triangle_area;
                        Real b1_camera = area(p_camera, p0_camera, p2_camera) / triangle_area;
                        Real b2_camera = area(p_camera, p1_camera, p0_camera) / triangle_area;

                        if (b0_camera < 0 || b1_camera < 0 || b2_camera < 0 || std::abs(b0_camera + b1_camera + b2_camera - 1) > 1e-9){
                            continue;
                        }

                        Real common_deniminator = b0_camera / p0.z + b1_camera / p1.z + b2_camera / p2.z;
                        Real b0 = (b0_camera / p0.z) / common_deniminator;
                        Real b1 = (b1_camera / p1.z) / common_deniminator;
                        Real b2 = (b2_camera / p2.z) / common_deniminator;

                        Real depth = b0 * p0.z + b1 * p1.z + b2 * p2.z;
                        if (z_min < depth){
                            z_min = depth;
                            color = mesh.face_colors[idx];
                        }
                    }
                    accumulated_color += color;
                }
            }
            img(x, y) = accumulated_color * weight;
        }
    }

    return img;
}

Image3 hw_2_3(const std::vector<std::string> &params) {
    // Homework 2.3: render a triangle mesh with vertex colors

    Image3 img(640 /* width */, 480 /* height */);
    Real alpha = Real(img.width) / img.height;

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{0.5, 0.5, 0.5};
        }
    }

    const int num_subpixels = 4;
    Real weight = 1.0 / (num_subpixels * num_subpixels);
    for (int x = 0; x < img.width; x++) {
        for (int y = 0; y < img.height; y++) {
            Vector3 accumulated_color{0, 0, 0};

            for (int i = 0; i < num_subpixels; i++) {
                for (int j = 0; j < num_subpixels; j++) {
                    Real sub_x = x + (i + 0.5) / num_subpixels;
                    Real sub_y = y + (j + 0.5) / num_subpixels;
                    Vector3 color = Vector3{0.5, 0.5, 0.5};

                    Vector3 p_camera((sub_x * 2 * s * alpha)/img.width - s * alpha, (1 - sub_y/img.height) * 2 * s - s, -1.0);
                    Real z_min = -infinity<Real>();

                    for (int idx = 0; idx < mesh.faces.size(); idx++) {
                        const Vector3i& face = mesh.faces[idx];
                        Vector3 p0 = mesh.vertices[face[0]];
                        Vector3 p1 = mesh.vertices[face[1]];
                        Vector3 p2 = mesh.vertices[face[2]];

                        if ((-z_near < p0.z) || (-z_near < p1.z) || (-z_near < p2.z)){
                            continue;
                        }

                        Vector3 p0_camera(-p0.x/p0.z, -p0.y/p0.z, -1.0);
                        Vector3 p1_camera(-p1.x/p1.z, -p1.y/p1.z, -1.0);
                        Vector3 p2_camera(-p2.x/p2.z, -p2.y/p2.z, -1.0);

                        Real triangle_area = area(p0_camera, p1_camera, p2_camera);
                        Real b0_camera = area(p_camera, p1_camera, p2_camera) / triangle_area;
                        Real b1_camera = area(p_camera, p0_camera, p2_camera) / triangle_area;
                        Real b2_camera = area(p_camera, p1_camera, p0_camera) / triangle_area;

                        if (b0_camera < 0 || b1_camera < 0 || b2_camera < 0 || std::abs(b0_camera + b1_camera + b2_camera - 1) > 1e-9){
                            continue;
                        }

                        Real common_deniminator = b0_camera / p0.z + b1_camera / p1.z + b2_camera / p2.z;
                        Real b0 = (b0_camera / p0.z) / common_deniminator;
                        Real b1 = (b1_camera / p1.z) / common_deniminator;
                        Real b2 = (b2_camera / p2.z) / common_deniminator;

                        Real depth = b0 * p0.z + b1 * p1.z + b2 * p2.z;
                        if (z_min < depth){
                            z_min = depth;
                            color = b0 * mesh.vertex_colors[face[0]] + b1 * mesh.vertex_colors[face[1]] + b2 * mesh.vertex_colors[face[2]];
                        }
                    }
                    accumulated_color += color;
                }
            }
            img(x, y) = accumulated_color * weight;
        }
    }

    return img;
}

Image3 hw_2_4(const std::vector<std::string> &params) {
    // Homework 2.4: render a scene with transformation
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.camera.resolution.x,
               scene.camera.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }

    Real alpha = Real(img.width) / img.height;
    Matrix4x4 view_matrix = inverse(scene.camera.cam_to_world);
    Real s = scene.camera.s;
    Real z_near = scene.camera.z_near;

    const int num_subpixels = 4;
    Real weight = 1.0 / (num_subpixels * num_subpixels);
    for (int x = 0; x < img.width; x++) {
        for (int y = 0; y < img.height; y++) {
            Vector3 accumulated_color{0, 0, 0};

            for (int i = 0; i < num_subpixels; i++) {
                for (int j = 0; j < num_subpixels; j++) {
                    Real sub_x = x + (i + 0.5) / num_subpixels;
                    Real sub_y = y + (j + 0.5) / num_subpixels;
                    Vector3 color = scene.background;

                    Vector3 p_camera((sub_x * 2 * s * alpha)/img.width - s * alpha, (1 - sub_y/img.height) * 2 * s - s, -1.0);
                    Real z_min = -infinity<Real>();

                    for (TriangleMesh mesh : scene.meshes) {
                        for (int idx = 0; idx < mesh.faces.size(); idx++) {
                            const Vector3i& face = mesh.faces[idx];
                            Vector3 p0_3 = mesh.vertices[face[0]];
                            Vector3 p1_3 = mesh.vertices[face[1]];
                            Vector3 p2_3 = mesh.vertices[face[2]];
                            
                            Vector4 p0_4 (p0_3[0], p0_3[1], p0_3[2], 1.0);
                            Vector4 p1_4 (p1_3[0], p1_3[1], p1_3[2], 1.0);
                            Vector4 p2_4 (p2_3[0], p2_3[1], p2_3[2], 1.0);

                            Vector4 p0 = view_matrix * mesh.model_matrix * p0_4;
                            Vector4 p1 = view_matrix * mesh.model_matrix * p1_4;
                            Vector4 p2 = view_matrix * mesh.model_matrix * p2_4;

                            if ((-z_near < p0.z) || (-z_near < p1.z) || (-z_near < p2.z)){
                                continue;
                            }

                            Vector3 p0_camera(-p0.x/p0.z, -p0.y/p0.z, -1.0);
                            Vector3 p1_camera(-p1.x/p1.z, -p1.y/p1.z, -1.0);
                            Vector3 p2_camera(-p2.x/p2.z, -p2.y/p2.z, -1.0);

                            Real triangle_area = area(p0_camera, p1_camera, p2_camera);
                            Real b0_camera = area(p_camera, p1_camera, p2_camera) / triangle_area;
                            Real b1_camera = area(p_camera, p0_camera, p2_camera) / triangle_area;
                            Real b2_camera = area(p_camera, p1_camera, p0_camera) / triangle_area;

                            if (b0_camera < 0 || b1_camera < 0 || b2_camera < 0 || std::abs(b0_camera + b1_camera + b2_camera - 1) > 1e-9){
                                continue;
                            }

                            Real common_deniminator = b0_camera / p0.z + b1_camera / p1.z + b2_camera / p2.z;
                            Real b0 = (b0_camera / p0.z) / common_deniminator;
                            Real b1 = (b1_camera / p1.z) / common_deniminator;
                            Real b2 = (b2_camera / p2.z) / common_deniminator;

                            Real depth = b0 * p0.z + b1 * p1.z + b2 * p2.z;
                            if (z_min < depth){
                                z_min = depth;
                                color = b0 * mesh.vertex_colors[face[0]] + b1 * mesh.vertex_colors[face[1]] + b2 * mesh.vertex_colors[face[2]];
                            }
                        }
                    }
                    accumulated_color += color;
                }
            }
            img(x, y) = accumulated_color * weight;
        }
    }

    return img;
}

