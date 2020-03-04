// TODO: Remove true fullscreen.
// TODO: Make window resize maintain aspect ratio

// The idea:
// A C++ game engine with integrated editor. Includes mapping/modeling/scripting/debugging in-game.
// Scripting via Java and/or BASIC (a BASIC-flavored Java-compatible dynamic language for the JVM).

// @FunctionalInterface
// public interface LoadSpriteFilesCallbackFunc {
//     public void cb();
// }

public class Game {
    private static native void print(String str);
    private static native void loadSpriteFile(String name, String filePath);
    private static native void loadSpriteFiles(String[] spriteNames, String[] spriteFilePaths, Runnable cb);
    // private static native void loadSpriteFiles(String[] spriteNames, String[] spriteFilePaths, LoadSpriteFilesCallbackFunc cb);
    private static native void createWidget(String name, String[] spriteNames, float[][] spriteLocations, float x, float y, float w, float h);
    private static boolean showedSprite = false;
    public static void main() {
        if (!showedSprite) {
            print("HELLO FROM JAVA 1\r\n");
            print("HELLO FROM JAVA 2\r\n");
            // LoadSpriteFilesCallbackFunc cb = () -> {
            //     // print("HELLO FROM JAVA\r\n");
            // };
            showedSprite = true;
            // TODO: Add fire-and-forget texture loading (on a separate thread?). Polling. Callback on success/failed?
            String[] spriteNames = new String[] {
                "kitten",
                "kitten2",
                "kitten3",
                "kitten4",
                "kitten5"
            };
            String[] spriteFilePaths = new String[] {
                "kitten.png",
                "kitten2.png",
                "kitten3.png",
                "kitten4.png",
                "kitten5.png"
            };
            // loadSpriteFiles(spriteNames, spriteFilePaths, null);
            // loadSpriteFiles(spriteNames, spriteFilePaths, new LoadSpriteFilesCallbackFunc() {
            //     @Override
            //     public void cb() {
            //         print("HELLO FROM JAVA 2\r\n");
            //     }
            // });
            loadSpriteFiles(spriteNames, spriteFilePaths, () -> {});
            // loadSpriteFiles(spriteNames, spriteFilePaths, new LoadSpriteFilesCallbackFunc() {
            //     @Override
            //     public void cb() {
            //         // print("HELLO FROM JAVA 2\r\n");
            //     }
            //     // float[][] spritePositions = new float[][] {
            //     //     new float[] { -0.25f, 0.25f, 0.5f, 0.5f },
            //     //     new float[] { -1.0f, 1.0f, 0.5f, 0.5f },
            //     //     new float[] { 0.5f, -0.5f, 0.5f, 0.5f },
            //     //     new float[] { -1.0f, -0.5f, 0.5f, 0.5f },
            //     //     new float[] { 0.5f, 1.0f, 0.5f, 0.5f }
            //     // };
            //     // // TODO: Support creating widget before named textures are loaded, auto-add them once loaded.
            //     // createWidget("kittens", spriteNames, spritePositions, 0.0f, 0.0f, 0.5f, 0.5f);
            //     // createWidget("kittens2", spriteNames, spritePositions, 1.0f, 1.0f, 0.5f, 0.5f);
            //     // createWidget("kittens2", spriteNames, spritePositions, 1.25f, 0.75f, 0.5f, 0.5f);
            // });
            // float[][] spritePositions = new float[][] {
            //     new float[] { 0.0f, 0.0f, 0.5f, 0.5f },
            //     new float[] { -0.5f, 0.5f, 0.5f, 0.5f },
            //     new float[] { 0.0f, 0.5f, 0.5f, 0.5f },
            //     new float[] { -0.5f, 0.0f, 0.5f, 0.5f },
            //     new float[] { -1.0f, 0.0f, 0.5f, 0.5f }
            // // };
            float[][] spritePositions = new float[][] {
                new float[] { -0.25f, 0.25f, 0.5f, 0.5f },
                new float[] { -1.0f, 1.0f, 0.5f, 0.5f },
                new float[] { 0.5f, -0.5f, 0.5f, 0.5f },
                new float[] { -1.0f, -0.5f, 0.5f, 0.5f },
                new float[] { 0.5f, 1.0f, 0.5f, 0.5f }
            };
            // TODO: Support creating widget before named textures are loaded, auto-add them once loaded.
            createWidget("kittens", spriteNames, spritePositions, 0.0f, 0.0f, 0.5f, 0.5f);
            createWidget("kittens2", spriteNames, spritePositions, 1.0f, 1.0f, 0.5f, 0.5f);
            createWidget("kittens2", spriteNames, spritePositions, 1.25f, 0.75f, 0.5f, 0.5f);
        }
    }
}
            // loadSpriteFile("kitten", "kitten.png");
            // loadSpriteFile("kitten2", "kitten2.png");
            // loadSpriteFile("kitten3", "kitten3.png");
            // loadSpriteFile("kitten4", "kitten4.png");
            // loadSpriteFile("kitten5", "kitten5.png");
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
