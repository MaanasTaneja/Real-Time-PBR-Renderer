# OpenGL Project - PBR Engine with Image Based Lighting

## Overview

Welcome to the culmination of advanced graphics programming—an OpenGL project that not only showcases a robust Image Based Lighting (IBL) Physically Based Rendering (PBR) engine but also integrates a sophisticated terrain renderer, a versatile postprocessor, and a compelling game designed to highlight the engine's capabilities.

## Physically Based Rendering (PBR) Implementation

The PBR aspect of the engine is implemented through the Cook-Torrance BRDF model, providing a realistic simulation of light-material interaction. Image Based Lighting (IBL) using HDR cubemaps enhances the rendering of various materials, including metallic and diffuse. The implementation draws inspiration from the frostbite graphics paper, ensuring a high-fidelity representation.

## Terrain Renderer

Dynamically generating vertex points at runtime, the terrain renderer creates a dynamic landscape with texture and normal coordinates calculated on-the-fly. Utilizing Perlin noise, Simplex noise, and other noise functions, the terrain's elevation is crafted, adding intricate details and dimension.

## Postprocessor

The postprocessor introduces a layer of visual enhancement, offering the flexibility to implement various effects. In this iteration, the focus is on tonemapping to the ACESfilm color space, contributing to a cinematic and vibrant display.

## Game Showcase

As a testament to the engine's capabilities, a small shooter game has been created. This game features:
- **Dynamic Player Controller:** The player is equipped with a dynamic controller, offering smooth and responsive movement.
- **Weapon System:** A simple yet effective weapon system utilizes the Bullet Physics engine to shoot projectiles, and the particle system adds visual flair to the shooting experience.
- **Level Design:** The game environment is loaded through the glTF format, showcasing the engine's ability to handle complex 3D models.
- **Shader Techniques:** The level is shaded using point lights, emphasizing realistic lighting effects. Image Based Lighting (IBL) is seamlessly integrated, leveraging an HDR skybox to enhance the overall visual experience.

## Disclaimer

While the project's code structure may initially reflect Java-like practices, ongoing iterations actively address these issues, aligning the codebase with industry-standard C++ practices. For comprehensive details on implementation and usage, refer to the provided documentation. Dive into the source code, experiment with the features, and enjoy the immersive world this engine and game combination have to offer.

Happy coding and gaming!
