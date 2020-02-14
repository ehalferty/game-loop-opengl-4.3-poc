public class Game {
    private static native void drawSprite(String name, float x, float y, float w, float h);
    private static boolean showedSprite = false;
    public static void main() {
        if (!showedSprite) {
            drawSprite("kitten", 0.0f, 0.0f, 0.5f, 0.5f);
            showedSprite = true;
        }
    }
}
