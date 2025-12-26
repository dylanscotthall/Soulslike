#version 450

layout(location = 0) in vec3 inPosition;  // use vec3 for 3D positions
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

// Uniform buffers
layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
} camera;

layout(set = 0, binding = 1) uniform ModelUBO {
    mat4 model;
} model;

void main() {
    gl_Position = camera.proj * camera.view * model.model * vec4(inPosition, 1.0);
    // gl_Position = model.model * vec4(inPosition, 1.0);
    fragColor = inColor;
}
