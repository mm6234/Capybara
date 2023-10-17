# Capybara

## Setting up the Environment

Follow [this guide](https://everythingtech.dev/2023/06/step-by-step-guide-on-how-to-get-started-with-drogon-on-windows-with-visual-studio/) to set up the environment. A few things that differs from the tutorial:
* Step 3.2: Clone the `Capybara` repo instead. 
* Step 3.4: Open CMakePresets.json and update your local directory to `vcpkg.cmake`. Everyone's Use two slashes `\\` instead. 
* Step 3.5 and 3.6: Do not need to modify; the change should be reflected in the repo. 
* Create a new file named `CMakePresets.json` at the root directory. Copy the text and change the directory (see appendix). 
* If you encounter issues in step 2.7, try uninstall `cmake` and try again. 

After the tutorial, compile the code and start the exe file. Use tools such as postman or curl to test the program. An example API call:
> http://localhost:6969/api/doctor-info/1
```
{
    "status": "200"
}
```

## Acknowledgement
* `.gitignore` file template from github's [gitignore repo](https://github.com/github/gitignore/blob/main/VisualStudio.gitignore). Used for best practices. 

## Appendix
`CMakePresets.json`: Change `toolchainFile` to the approptiate directory. For example: `"C:\\Users\\iamyo\\projects\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake"`
```
{
    "version": 3,
    "configurePresets": [
        {
            "name": "windows-base",
            "hidden": true,
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/out/build/${presetName}",
            "installDir": "${sourceDir}/out/install/${presetName}",
            "cacheVariables": {
                "CMAKE_C_COMPILER": "cl.exe",
                "CMAKE_CXX_COMPILER": "cl.exe"
            },
            "condition": {
                "type": "equals",
                "lhs": "${hostSystemName}",
                "rhs": "Windows"
            }
        },
      {
        "name": "x64-debug",
        "displayName": "x64 Debug",
        "inherits": "windows-base",
        "architecture": {
          "value": "x64",
          "strategy": "external"
        },
        "cacheVariables": {
          "CMAKE_BUILD_TYPE": "Debug"
        },
        "toolchainFile": "<Insert your directory to vcpkg.cmake>"
      },
        {
            "name": "x64-release",
            "displayName": "x64 Release",
            "inherits": "x64-debug",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release"
            }
        },
        {
            "name": "x86-debug",
            "displayName": "x86 Debug",
            "inherits": "windows-base",
            "architecture": {
                "value": "x86",
                "strategy": "external"
            },
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug"
            }
        },
        {
            "name": "x86-release",
            "displayName": "x86 Release",
            "inherits": "x86-debug",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release"
            }
        },
        {
            "name": "linux-debug",
            "displayName": "Linux Debug",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/out/build/${presetName}",
            "installDir": "${sourceDir}/out/install/${presetName}",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug"
            },
            "condition": {
                "type": "equals",
                "lhs": "${hostSystemName}",
                "rhs": "Linux"
            },
            "vendor": {
                "microsoft.com/VisualStudioRemoteSettings/CMake/1.0": {
                    "sourceDir": "$env{HOME}/.vs/$ms{projectDirName}"
                }
            }
        },
        {
            "name": "macos-debug",
            "displayName": "macOS Debug",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/out/build/${presetName}",
            "installDir": "${sourceDir}/out/install/${presetName}",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug"
            },
            "condition": {
                "type": "equals",
                "lhs": "${hostSystemName}",
                "rhs": "Darwin"
            },
            "vendor": {
                "microsoft.com/VisualStudioRemoteSettings/CMake/1.0": {
                    "sourceDir": "$env{HOME}/.vs/$ms{projectDirName}"
                }
            }
        }
    ]
}

```