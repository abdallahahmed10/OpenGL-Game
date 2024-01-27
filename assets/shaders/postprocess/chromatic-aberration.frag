#version 330

// The texture holding the scene pixels
uniform sampler2D tex;

// Read "assets/shaders/fullscreen.vert" to know what "tex_coord" holds;
in vec2 tex_coord;
out vec4 frag_color;

// How far (in the texture space) is the distance (on the x-axis) between
// the pixels from which the red/green (or green/blue) channels are sampled
#define STRENGTH 0.005

// Chromatic aberration mimics some old cameras where the lens disperses light
// differently based on its wavelength. In this shader, we will implement a
// cheap version of that effect 

void main(){
    //TODO: Modify this shader to apply chromatic abberation
    // To apply this effect, we only read the green channel from the correct pixel (as defined by tex_coord)
    // To get the red channel, we move by amount STRENGTH to the left then sample another pixel from which we take the red channel
    // To get the blue channel, we move by amount STRENGTH to the right then sample another pixel from which we take the blue channel
    // Sample the green channel from the current texture coordinate
    float green = texture(tex, tex_coord).g;
    
    // Offset the texture coordinate by STRENGTH to the left for the red channel
    vec2 redCoord = tex_coord - vec2(STRENGTH, 0.0);
    float red = texture(tex, redCoord).r;
    
    // Offset the texture coordinate by STRENGTH to the right for the blue channel
    vec2 blueCoord = tex_coord + vec2(STRENGTH, 0.0);
    float blue = texture(tex, blueCoord).b;
    
    // Combine the three channels
    frag_color = vec4(red, green, blue, 1.0);
    //frag_color = texture(tex, tex_coord);
}