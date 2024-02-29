cd android
call gradlew build
adb install -r app/build/outputs/apk/debug/app-debug.apk
adb shell am start -n com.example.test/.MainActivity
cd ..