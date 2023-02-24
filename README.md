# canvas-drawer

Implements a simple drawing API that supports lines, triangles, circles, rose curves, and Maurer roses

![exhibit2](https://user-images.githubusercontent.com/75283980/221119758-4d685e68-9d7b-4b12-a70e-23ccf1ccb5c1.png)

## How to build

*Windows*

Open git bash to the directory containing this repository.

```
canvas-drawer $ mkdir build
canvas-drawer $ cd build
canvas-drawer/build $ cmake -G "Visual Studio 16 2019" ..
canvas-drawer/build $ start Draw-2D.sln
```

Your solution file should contain two projects: `pixmap_art` and `pixmap_test`.
To run from the git bash command shell, 

```
canvas-drawer/build $ ../bin/Debug/draw_test
canvas-drawer/build $ ../bin/Debug/draw_art
```

*macOS*

Open terminal to the directory containing this repository.

```
canvas-drawer $ mkdir build
canvas-drawer $ cd build
canvas-drawer/build $ cmake ..
canvas-drawer/build $ make
```

To run each program from build, you would type

```
canvas-drawer/build $ ../bin/draw_test
canvas-drawer/build $ ../bin/draw_art
```

## Supported Primitives

### Lines with Solid or Interpolated Color

![line-color-interpolation](https://user-images.githubusercontent.com/75283980/221120377-3bb1f8ac-c43d-4b92-8efc-0ac7b6203d42.png)
![diagonal-line-1](https://user-images.githubusercontent.com/75283980/221120581-873dfbcd-6d0e-45f3-9da9-27b9a9a16892.png)![diagonal-line-2](https://user-images.githubusercontent.com/75283980/221120435-80fd3019-a6ae-4c05-a06e-0dfa61578baa.png)![vertical-line](https://user-images.githubusercontent.com/75283980/221120497-57b84d12-2f08-431c-bc0e-18b72c7e1fc2.png)![horizontal-line](https://user-images.githubusercontent.com/75283980/221120628-86abfe44-bbb3-4fbd-8e85-5c8a4837b829.png)![h-lessthan-w-line-1](https://user-images.githubusercontent.com/75283980/221120657-2900634c-6ee7-406c-b3a0-2c90290bc155.png)![h-lessthan-w-line-2](https://user-images.githubusercontent.com/75283980/221120665-5f816a96-e74c-4cb1-9f68-b493bd7dc7a8.png)![w-lessthan-h-line-1](https://user-images.githubusercontent.com/75283980/221120685-74e32ed8-7048-4807-b1d6-8c9113f86967.png)![w-lessthan-h-line-2](https://user-images.githubusercontent.com/75283980/221120691-f6fed59c-5c62-4332-8757-e1a606ca5801.png)

### Triangles with Solid or Interpolated Color

![quad](https://user-images.githubusercontent.com/75283980/221121014-132577a5-e726-4651-b29d-b262d991ba33.png)![triangle](https://user-images.githubusercontent.com/75283980/221121039-023330ab-5344-4878-97f9-700519d6efa0.png)

### Circles

![test_circle](https://user-images.githubusercontent.com/75283980/221121171-d0210960-151d-4438-96bd-224f91f09e84.png)

### Filled or Empty Circles and Triangles

![test_outline_circle](https://user-images.githubusercontent.com/75283980/221121254-47195923-dfaf-4892-9c37-b05cbd8b3ec1.png)![test_outline_triangle](https://user-images.githubusercontent.com/75283980/221121281-b4794773-b9f0-4a00-b6a7-bd7c05b65520.png)

### Rose Curves

![test_rose](https://user-images.githubusercontent.com/75283980/221121344-3816896d-ae2f-46d2-bc55-a917c3641b87.png)

### Maurer Roses

![test_maurer](https://user-images.githubusercontent.com/75283980/221121397-77d10eec-8ccc-4743-b26f-7e7266d549ad.png)

## Results
![exhibit1](https://user-images.githubusercontent.com/75283980/221119961-02195e7e-b726-445d-91ea-4a0c348f23d8.png)
![exhibit2](https://user-images.githubusercontent.com/75283980/221119969-1484461c-248c-4b35-af2b-33d281edb2c9.png)
![exhibit3](https://user-images.githubusercontent.com/75283980/221119978-a35e7f0d-c6b7-4e6d-af59-86156daf86e2.png)

