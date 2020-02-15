public class Game {
    private static native void drawSprite(String name, float x, float y, float w, float h);
    private static boolean showedSprite = false;
    public static void main() {
        if (!showedSprite) {
            drawSprite("kitten.png", 0.0f, 0.0f, 0.75f, 0.1f);
            drawSprite("kitten2.png", 0.0f, 0.0f, 1.0f, 1.0f);
            drawSprite("kitten2.png", -1.0f, 1.0f, 0.25f, 0.25f);
            drawSprite("kitten2.png", -0.75f, 0.75f, 0.25f, 0.25f);
            drawSprite("kitten.png", -0.50f, 0.5f, 0.25f, 0.25f);
            drawSprite("kitten2.png", -0.25f, 0.25f, 0.25f, 0.25f);
            showedSprite = true;
        }
    }
}
