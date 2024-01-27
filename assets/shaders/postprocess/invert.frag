// #version 330

// uniform sampler2D tex;
// in vec2 tex_coord;
// out vec4 frag_color;

// void main() {
//     vec4 sceneColor = texture(tex, tex_coord);
//     frag_color = vec4(1.0) - sceneColor;
// }

// #version 330

// uniform sampler2D tex;
// in vec2 tex_coord;
// out vec4 frag_color;

// void main() {
//     vec4 sceneColor = texture(tex, tex_coord);
//     float gray = dot(sceneColor.rgb, vec3(0.299, 0.587, 0.114));
//     frag_color = vec4(vec3(gray), sceneColor.a);
// }


#version 330

uniform sampler2D tex;
in vec2 tex_coord;
out vec4 frag_color;

void main() {
    vec4 sceneColor = texture(tex, tex_coord);
    frag_color = vec4(sceneColor.r, sceneColor.g * 0.5, sceneColor.b * 0.5, sceneColor.a);
}



// #version 330

// uniform sampler2D tex;
// uniform float time;             // Pass a time variable that changes per frame.
// in vec2 tex_coord;
// out vec4 frag_color;

// // Function to generate random values based on pixel coordinates and time
// float rand(vec2 co){
//     return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453 + time);
// }

// void main() {
//     vec4 sceneColor = texture(tex, tex_coord);
    
//     // Generate the film grain intensity
//     float grainAmount = rand(tex_coord + time) * 0.05; // Adjust the 0.05 value to change the intensity of the grain

//     // Overlay the grain onto the scene color and make sure it does not exceed 1.0
//     vec3 grainOverlay = vec3(grainAmount);
    
//     // Mixing the original scene color with the grain, lower factor leads to lower intensity grain
//     vec3 finalColor = mix(sceneColor.rgb, sceneColor.rgb + grainOverlay, 0.5); // Adjust the 0.1 to change intensity

//     frag_color = vec4(finalColor, sceneColor.a);
// }