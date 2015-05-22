attribute vec4 vPosition;
uniform mat4 p;
uniform mat4 mv;
void
main()
{
    gl_Position = p*mv*vPosition;
}
