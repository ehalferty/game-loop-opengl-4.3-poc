public class Game {
    private static native void loadSpriteFile(String name, String filePath);
    private static native void createWidget(String name, String[] spriteNames, float[][] spriteLocations, float x, float y, float w, float h);
    private static boolean showedSprite = false;
    public static void main() {
        if (!showedSprite) {
            showedSprite = true;
            loadSpriteFile("kitten", "kitten.png");
            loadSpriteFile("kitten2", "kitten2.png");
            loadSpriteFile("kitten3", "kitten3.png");
            loadSpriteFile("kitten4", "kitten4.png");
            loadSpriteFile("kitten5", "kitten5.png");
            String[] spriteNames = new String[] {
                "kitten",
                "kitten2",
                "kitten3",
                "kitten4",
                "kitten5"
            };
            float[][] spritePositions = new float[][] {
                new float[] { -0.5f, -0.5f, 0.25f, 0.25f },
                new float[] { 0.0f, 0.0f, 0.25f, 0.25f },
                new float[] { 0.5f, 0.5f, 0.25f, 0.25f },
                new float[] { 0.75f, 0.75f, 0.1f, 0.1f },
                new float[] { 0.85f, 0.85f, 0.1f, 0.1f }
            };
            createWidget("kittens", spriteNames, spritePositions, 0.0f, 0.0f, 0.5f, 0.5f);
        }
    }
}
// public class Game {
//     private static native void loadSpriteFile(String name, String filePath);
//     private static native void createWidget(String name, String[] spriteNames, float[][] spriteLocations, float x, float y, float w, float h);
//     private static boolean showedSprite = false;
//     public static void main() {
//         if (!showedSprite) {
//             showedSprite = true;
//             loadSpriteFile("kitten", "kitten.png");
//             loadSpriteFile("kitten2", "kitten2.png");
//             loadSpriteFile("kitten3", "kitten3.png");
//             loadSpriteFile("kitten4", "kitten4.png");
//             loadSpriteFile("kitten5", "kitten5.png");
//             String[] spriteNames = new String[] {
//                 "kitten",
//                 "kitten2",
//                 "kitten3",
//                 "kitten4",
//                 "kitten5"
//             };
//             float[][] spritePositions = new float[][] {
//                 new float[] { -1.0f, -1.0f, 0.25f, 0.25f },
//                 new float[] { -0.75f, -0.75f, 0.5f, 0.5f },
//                 new float[] { -0.25f, -0.25f, 0.25f, 0.25f },
//                 new float[] { 0.0f, 0.0f, 0.5f, 0.5f },
//                 new float[] { 0.5f, 0.5f, 0.5f, 0.5f }
//             };
//             createWidget("kittens", spriteNames, spritePositions, 0.0f, 0.0f, 0.5f, 0.5f);
//         }
//     }
// }
