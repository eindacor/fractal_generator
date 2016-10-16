#version 430
in vec4 fragment_color;
flat in int discard_fragment;
out vec4 output_color;
void main()
{
	if (discard_fragment == 1)
		discard;

	else output_color = fragment_color;
}