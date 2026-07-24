# 2D Particle Fuild Simulation (SPH)
This is a personal university project where I attempted to make an SPH (Smoothed Particle Hydrodynamics) simulation. It is mostly based on [this paper](https://matthias-research.github.io/pages/publications/sca03.pdf) (Müller, Charypar, Gross. 2003) and [Sebastian Lague's implementation in unity](https://github.com/SebLague/Fluid-Sim). Except I tried to make this in C++ and OpenGL to generalise it and learn more on how it could connect to an engine on it's own. Besides learning how to make a fluid simulation, the goal of this project was also to do a benchmark and analysis on optimisations. In this case I learned how to write an OpenGL compute shader containing all the simulation math and handle communication between the CPU and GPU. The benchmark clearly confirmed a reduction in time complexity of $O(n^2)$ to $O(n)$ when using the compute shaders, without any major error points or weird behaviour. Please check out the Wiki page in the top bar for details about the implementation, benchmarks, tests, analysis and research.

The particle Simulation runs smoothly up to around 5000 particles under most standard simulation parameters. Beyond this amount the simulation clearly struggles to stabilize itself both in fps and simulation state, hinting at a need for more optimisations.

Futere steps for this project include region based optimisation for a huge boost in performance, a fluid visualisation using marching squares, an upgrade to 3D, and making the fluid behave even more realistically using viscosity and surface tension. Something I am also curious about is how it would be possible to make the fluid interact with dynamic boundaries and meshes, as that is something for which I cannot yet form a clear idea.

You can try to download and build the current implementation yourself using cmake but you might need to install [vcpkg](https://vcpkg.io/en/) together with glad, GLFW, glm and ImGui.

Examples of the fluid simulation running with different gravity and texel density parameters:

https://github.com/user-attachments/assets/f14f4727-adfd-4f33-8013-a144ff3af966

https://github.com/user-attachments/assets/23552bb0-f24b-4951-b059-199170036ec6

## Personal Notes
I am really proud of the result and it's almost exactly as I hoped it would be. Playing around with the fluid is really satisfying and has propably distracted me for multiple hours in total during the development. Seeing the particles behave more and more like a real fluid with every iteration was very rewarding. I am not sure yet if I will improve further upon the simulation since there are other projects that will take my priority for now.

## ScreenShots
<img width="100%" alt="PurpleYellow" src="https://github.com/user-attachments/assets/6da8fe1a-0d3d-4a47-9200-bcda9479ea12" />
<img width="100%" alt="PinkWave" src="https://github.com/user-attachments/assets/96808cf4-dad8-4b89-b08f-064473ff53d9" />
<img width="100%" alt="PinkPastel" src="https://github.com/user-attachments/assets/49f5ccae-2925-47be-af87-6085bab9698c" />
<img width="100%" alt="Lava" src="https://github.com/user-attachments/assets/1cb3f33c-c647-4be6-b4d0-b75f248107b6" />
<img width="100%" alt="GreenWave" src="https://github.com/user-attachments/assets/dae43b8f-bbfc-49b4-b3dc-2fd1d6c0adcf" />
<img width="100%" alt="Foamy" src="https://github.com/user-attachments/assets/0bd139ea-2173-467b-9c72-86de3006049f" />
<img width="100%" alt="BlueJellyFish" src="https://github.com/user-attachments/assets/0a33aaea-df7c-440c-bcb0-c29fee891610" />
<img width="100%" alt="BlackSpace" src="https://github.com/user-attachments/assets/5ffe8a80-1680-4734-84b8-5e9b3d2942ff" />
<img width="100%" alt="Ball" src="https://github.com/user-attachments/assets/ed2510a8-f336-43aa-9d58-d907b9324236" />
<img width="100%" alt="WhiteSpace" src="https://github.com/user-attachments/assets/445f2c21-bc6c-4651-983d-b913d820328f" />
<img width="100%" alt="WaveCrash" src="https://github.com/user-attachments/assets/f6e59263-1d71-4da0-bca7-1be1ec3e2462" />
