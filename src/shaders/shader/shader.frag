#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex;

vec4 mandelbrotSet(vec4 fragCoord);

void main()
{
    FragColor = texture(tex, TexCoord);
    //FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    //FragColor = mandelbrotSet(gl_FragCoord);
}

vec4 mandelbrotSet(vec4 fragCoord) {

    float ca = (fragCoord.x - 400) / 200.0f;
    float cb = (fragCoord.y - 300) / 200.0f;

    float za = 0;
    float zb = 0;

    float max = 40;
    float i = 0;

    while (za * za + zb * zb < 50 && i < max) {

        float temp = za;
        temp = za * za - zb * zb;
        zb = 2 * za * zb;
        za = temp;

        za += ca;
        zb += cb;

        i++;
    }

    float ratio = i / max;

    return vec4(ratio, ratio, ratio, 1.0f);
}
