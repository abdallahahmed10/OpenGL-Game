#version 330 core

in Varyings {
    vec4 color;
    vec2 tex_coord;
} fs_in;

out vec4 frag_color;

uniform vec4 tint;
uniform sampler2D tex;

void main(){
    //TODO: (Req 7) Modify the following line to compute the fragment color
    // by multiplying the tint with the vertex color and with the texture color 
    // Sample the texture color using the provided texture coordinates
    vec4 textureColor = texture(tex, fs_in.tex_coord);

    // Compute the fragment color by multiplying the tint with the vertex color and the texture color
    frag_color = fs_in.color * tint * textureColor;

}