public class Game {
    private static native void loadSpriteFile(String name, String filePath);
    private static native void createWidget(String name, String[] spriteNames, Float[][] spriteLocations, Float x, Float y, Float w, Float h);



    // private static native void drawSprite(String name, float x, float y, float w, float h);
    private static boolean showedSprite = false;
    public static void main() {
        if (!showedSprite) {
            loadSpriteFile("kitten", "kitten.png");
            loadSpriteFile("kitten2", "kitten2.png");
            Float[][] positions = new Float[][] {
                new Float[] { 0.0f, 0.0f, 0.75f, 0.1f },
                new Float[] { 0.0f, 0.0f, 1.0f, 1.0f }
            };
            createWidget("kittens", new String[] { "kitten", "kitten2" }, positions, 0.0f, 0.0f, 0.5f, 0.5f);

            // drawSprite("kitten.png", 0.0f, 0.0f, 0.75f, 0.1f);
            // drawSprite("kitten2.png", 0.0f, 0.0f, 1.0f, 1.0f);
            // drawSprite("kitten2.png", -1.0f, 1.0f, 0.25f, 0.25f);
            // drawSprite("kitten2.png", -0.75f, 0.75f, 0.25f, 0.25f);
            // drawSprite("kitten.png", -0.50f, 0.5f, 0.25f, 0.25f);
            // drawSprite("kitten2.png", -0.25f, 0.25f, 0.25f, 0.25f);


            // String[] fileNames = new String[6];
            // fileNames[0] = "kitten2.png";
            // fileNames[1] = "kitten.png";
            // fileNames[2] = "kitten.png";
            // fileNames[3] = "kitten.png";
            // fileNames[4] = "kitten.png";
            // fileNames[5] = "kitten2.png";
            // Float[] xs = new Float[6];
            // xs[0] = 0.0f;
            // xs[1] = 0.0f;
            // xs[2] = -1.0f;
            // xs[3] = -0.75f;
            // xs[4] = -0.5f;
            // xs[5] = -0.25f;
            // Float[] ys = new Float[6];
            // ys[0] = 0.0f;
            // ys[1] = 0.0f;
            // ys[2] = 1.0f;
            // ys[3] = 0.75f;
            // ys[4] = 0.5f;
            // ys[5] = 0.25f;
            // Float[] ws = new Float[6];
            // ws[0] = 0.75f;
            // ws[1] = 1.0f;
            // ws[2] = 0.25f;
            // ws[3] = 0.25f;
            // ws[4] = 0.25f;
            // ws[5] = 0.25f;
            // Float[] hs = new Float[6];
            // hs[0] = 0.1f;
            // hs[1] = 1.0f;
            // hs[2] = 0.25f;
            // hs[3] = 0.25f;
            // hs[4] = 0.25f;
            // hs[5] = 0.25f;

            // for (int i = 0; i < 6; i++) {
            //     drawSprite(fileNames[i], xs[i], ys[i], ws[i], hs[i]);
            // }

            // loadSpriteFile("kitten", "kitten.png");
            // loadSpriteFile("kitten2", "kitten2.png");
            // createWidget("kittens", )


            // drawSprite("kitten.png", 0.0f, 0.0f, 0.75f, 0.1f);
            // drawSprite("kitten2.png", 0.0f, 0.0f, 1.0f, 1.0f);
            // drawSprite("kitten2.png", -1.0f, 1.0f, 0.25f, 0.25f);
            // drawSprite("kitten2.png", -0.75f, 0.75f, 0.25f, 0.25f);
            // drawSprite("kitten.png", -0.50f, 0.5f, 0.25f, 0.25f);
            // drawSprite("kitten2.png", -0.25f, 0.25f, 0.25f, 0.25f);
            showedSprite = true;
        }
    }
}
