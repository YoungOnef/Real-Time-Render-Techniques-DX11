# Real-Time Rendering Techniques

This project showcases a 3D scene implementation using various rendering techniques to create stunning visual effects.

<img src="https://github.com/YoungOnef/Real-Time-Render-Techniques-DX11/assets/72264732/43ca7071-8038-4907-8e3a-5c9427598514" width="600" />

## Techniques

1. **Texture Mapping:** A technique that applies images to the surfaces of 3D objects to make them look more realistic and detailed. The project uses anisotropic filtering to improve the quality of texture mapping and normal mapping to create the illusion of depth on the castle walls.

<img src="https://github.com/YoungOnef/Real-Time-Render-Techniques-DX11/assets/72264732/20de115c-f015-44d3-b6f5-6cb2f79ee81f" width="600" />

2. **Water Effects:** A technique that simulates the appearance and movement of water on the surface of a grid. The project uses a Fresnel-based reflection model to determine the amount of reflection on the water based on the eye angle.

<img src="https://github.com/YoungOnef/Real-Time-Render-Techniques-DX11/assets/72264732/3fcfc6f0-841d-45d5-a461-027209e67ee5" width="600" />

3. **Foliage Effects:** A technique that creates complex and realistic vegetation objects, such as grass and trees. The project uses grass shaders to create a texture of moving fur covering the tiles, and tree shaders to apply per-pixel lighting and a phong reflection model to the tree model.

<img src="https://github.com/YoungOnef/Real-Time-Render-Techniques-DX11/assets/72264732/742b79ea-2a42-4be9-be3c-617583a26740" width="600" />

4. **Particle System:** A technique that creates dynamic and animated objects, such as fire. The project uses billboards that always face the camera to render the fire particles.

<img src="https://github.com/YoungOnef/Real-Time-Render-Techniques-DX11/assets/72264732/0cd7efac-78ca-494e-afce-0cc49a44327f" width="600" />

5. **Lighting:** A technique that illuminates the scene based on the position and direction of light sources. The project uses per-pixel lighting shaders to calculate the color of each pixel based on the light source’s position.

<img src="https://github.com/YoungOnef/Real-Time-Render-Techniques-DX11/assets/72264732/7d5e8e32-05be-4cf7-ba68-59acf4ead2f2" width="600" />

6. **Glow Effects:** A technique that creates a bright and blurry effect around certain objects, such as the sword and the shark. The project uses emissive shaders and a blur utility class to create the post-processing effect.

<img src="https://github.com/YoungOnef/Real-Time-Render-Techniques-DX11/assets/72264732/7c52dc91-df02-436f-8310-52769e2018b5" width="600" />

## Performance Testing Results

<img src="https://github.com/YoungOnef/Real-Time-Render-Techniques-DX11/assets/72264732/a4ef1549-9d96-41dc-acb1-a3d82dce85cd" width="600" />

To assess the impact of each technique on the frame rate and memory usage, we conducted performance testing with the same scene and camera position. Here are the results:

| Technique            | FPS  | Memory (MB) | FPS Difference | Memory Difference |
|----------------------|------|-------------|----------------|-------------------|
| Texture mapping      | 440  | 328         | 31             | -5                |
| Water effects        | 431  | 330         | 22             | -3                |
| Foliage effects      | 1405 | 337         | 996            | 4                 |
| Particle system      | 441  | 342         | 32             | 9                 |
| Lighting             | 411  | 330         | 2              | -3                |
| Glow effects         | 421  | 331         | 12             | -2                |

## Conclusion

The Real-Time Rendering Techniques project successfully demonstrates the implementation and integration of various rendering techniques into a cohesive 3D scene. Each technique plays a crucial role in enhancing the visual quality and realism of the rendered objects. The performance testing results provide valuable insights into the impact of individual techniques on frame rate and memory usage, aiding in optimizing the scene for an optimal real-time experience.
