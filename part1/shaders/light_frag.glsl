#version 410 core
out vec4 color;

// Entry point of program
void main()
{
    // send fragment to output 
    // Default as yellow color that represents light source
	color = vec4(0.85f, 0.96f, 0.65f, 1.0f);
}