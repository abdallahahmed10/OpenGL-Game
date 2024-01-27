#version 330 core

layout(location = 0) in vec3 position; //states where each vertex attribute is stored in the Vertex buffer object
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord; //pt on the texture that our vertex will map from (texture image: uv space)
layout(location = 3) in vec3 normal;

//Varyings are "out" from a shader and "in" to another shader
//Send vertex information to the fragment shader
out Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
    vec3 view;
    vec3 world;
    vec3 fragpos;
    // vec3 eye;
} vs_out;


//Model matrix, transforms object from local to world
uniform mat4 M;

//Inverse transpose of model matrix, transforms the normal to the object (because non-uniform scaling messes with normal vectors)
uniform mat4 M_IT;

//projection matrix * the camera view matrix
uniform mat4 VP;



//Set varying data
void main()
{
    //Multiply local vertex position by the model matrix (transforming it to world coordinates)
    vec3 world = (M * vec4(position, 1.0f)).xyz; //Gets world coordinate of vertex

    //vertex's final position
    gl_Position = VP * vec4(world, 1.0);
    vs_out.color = color;

    vs_out.tex_coord = tex_coord;

    /*
    Transform normal vector of the vertex:
    non uniform scaling: use the model matrix inverse transpose
    uniform scaling: M is the same as M_IT
    normalize the vector:
    */
    vs_out.normal = normalize((M_IT * vec4(normal, 0.0)).xyz);

    // vs_out.view = eye - world; //view matrix: direction vector, from the camera to the object
    vs_out.world = world;
    //vs_out.fragpos = world;
}





