# 2D Particle Fuild Simulation (SPH)
This is a personal university project where I attempted to make an SPH (Smoothed Particle Hydrodynamics) simulation. It is mostly based on [Sebastian Lague's](https://github.com/SebLague/Fluid-Sim) implementation in unity. Except I tried to make this in C++ and OpenGL to generalise it and learn more on how it could connect to an engine on it's own. Besides learning how to make a fluid simulation, the goal of this project was also to do a benchmark and analysis on optimisations. In this case I learned how to write an OpenGL compute shader containing all the simulation math and handle communication between the CPU and GPU. The benchmark clearly confirmed a reduction in time complexity of $O(n^2)$ to $O(n)$ without any major error points or weird behaviour. Please check out the Wiki page in the top bar for details about the implementation, benchmarks, tests, analysis and research.

The particle Simulation runs smoothly up to around 3000 particles under most standard simulation parameters. After this amount the simulation clearly struggles to stableize itself both in fps and simulation state, hinting at a need for more optimisations.

Futere steps for this project include region based optimisation for a huge boost in performance, a fluid visualisation using marching squares, an upgrade to 3D, and making the fluid behave even more realistically using viscosity and surface tension. Something I am also curious about is how it would be possible to make the fluid interact with dynamic boundaries and meshes, as that is something for which I cannot yet form a clear idea.

You can try to download and build the current implementation yourself using cmake but you might need to install [vcpkg](https://vcpkg.io/en/) together with glad, GLFW, glm and ImGui.

Examples of the fluid simulation running with different gravity and texel density parameters.

https://github.com/user-attachments/assets/f14f4727-adfd-4f33-8013-a144ff3af966

https://github.com/user-attachments/assets/23552bb0-f24b-4951-b059-199170036ec6

### Personal Notes
I am really proud of the result and it's almost exactly as I hoped it would be. Playing around with the fluid is really satisfying and has propably distracted me for multiple hours in total during the development. Seeing the particles behave more and more like a real fluid with every iteration was very rewarding. I am not sure yet if I will improve further upon the simulation since there are other projects that will take my priority for now.
