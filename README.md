# circlearound
This simple sample shows how to use emscripten to build an app using OpenGL and then deploy it as a native application on Android and IOS

# Technology
This uses the following technologies:
- [emscripten with OpenGL](https://emscripten.org/docs/porting/multimedia_and_graphics/OpenGL-support.html) - This is used to convert the C++ code to [WebAssembly](https://en.wikipedia.org/wiki/WebAssembly)
- [Capacitor](https://capacitorjs.com/) - Cross platform native runtime for web apps.

# Setup Steps
1. (On Windows and Mac OS/X) [Install Android studio](https://developer.android.com/studio/install)
2. (On Mac OS/X) [Install XCode from the Mac App Store](https://apps.apple.com/us/app/xcode/id497799835?mt=12)
3. Set up an emscripten app so that it builds into the `dist/` directory with a main entry point `index.html`.  Note that you can modify this in the `capacitor.config.ts`, but it is easiest to start with the defaults.
4. Initialize `npm` in the project root directory if you haven't already done so with
    ```
    npm init
    ```
5. Install local standalone web server (serve)[https://www.npmjs.com/package/serve]
   ```
   npm install serve
   ```
6. Set up a default build command and possibly a serve command in `package.json` in the `scripts` section.  Change `appname` to match your source files:
   ```JSON
    "scripts": {
       "build": "emcc appname.cpp -o dist/index.html -s USE_WEBGL2=1 -s FULL_ES2=1 -O2 --shell-file appname.html",
       "serve": "serve dist"
    },
    ```
7. [Install Capacitor](https://capacitorjs.com/docs/getting-started)
   ```
   npm install @capacitor/core @capacitor/cli
   ```
8. [Initialize Capacitor](https://capacitorjs.com/docs/getting-started).  You only need to do this once.
   ```
   npx cap init
   ```
9. Add the Platforms you want to build for.  Note that while you can do both on a Mac, you can only do Android on a windows platform.
   ```
   npx cap add ios
   npx cap add android
   ```
10. Build your project so that the web pages are in the `dist/` directory.
    ```
    npm run build
    ```
11. If you want to test it locally, you can use serve:
    ```
    npm run serve
    ```
    and then open up the link (typically `http://localhost:3000`) to see the application run.
    
12. Synchronize the built code with the native project.  Basically it copies the compiled code into
the target directories.  You have to do this each time after you build the code.
    ```
    npx cap sync
    ```
13. Test it on the target device or emulator
    ```
    npx cap open ios
    npx cap open android
    ```
