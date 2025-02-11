# Heartheart

>A software and design project for the Coding IxD course of Winter Semester 24/25, by the FU Berlin and KH Berlin.

Heartheart transforms paper into an interactive sound interface, blending handcrafted techniques with technology to create tactile, multisensory experiences that redefine materiality and interaction.

![Title-heartheart](https://github.com/user-attachments/assets/3a12b216-b5ab-40c2-8fca-20773d36e90f)

## Table of Contents

### Introduction
Heartheart is an experimental project that redefines the role of paper as an interactive medium for sound creation. By merging traditional craftsmanship with modern technology, this project transforms sheets of paper into tactile sound interfaces. Through precise techniques such as embossing, texturing, and layering, Heartheart explores the untapped potential of paper as a dynamic, responsive surface.

### Concept & Objectives
The project challenges conventional perceptions of materiality and interaction by demonstrating how paper, a widely available and sustainable material, can be adapted into a functional interface. The primary goal is to encourage intuitive engagement, allowing users to explore sound through touch and movement. By embedding paper-based interfaces with a bare conductive touch board and the use of conductive ink, the project translates physical interactions into sound, creating a seamless blend of analog and digital experiences.

### Design & Interaction
Each design iteration in Heartheart highlights a unique method of interaction, providing diverse sensory experiences:
Layered Paper: Concentric cutouts form a tunnel-like effect, inviting users to swipe through layers, triggering distinct sounds.
Embossed Flowing Forms: Raised tactile shapes encourage users to trace their fingers, creating an immersive soundscape.
Orbital Circles: Circular embossed patterns mimic orbital movement, responding to touch with interactive sound elements.
Textured Semi-Circles: Lifting and layering techniques introduce depth and movement, allowing users to explore shifting textures and sounds.

### Significance & Impact
Heartheart presents a fresh perspective on interface design, demonstrating how a simple, organic material can serve as a bridge between the physical and digital worlds. By leveraging paper’s adaptability, the project opens new possibilities for sustainable, customizable, and interactive experiences. It invites designers, artists, and technologists to rethink the role of everyday materials in digital interaction, pushing the boundaries of how we engage with sound and touch.

### Usage
This project can be used to experiment with the design of interfaces made of paper, in order to create music through any music creation program, such as Ableton, or even with synthesizers that can be found for free on the web. 

To realize the different interfaces, we have used conductive ink on the back of the paper that allows us to read the stimulus of the hand on the paper. The next step will be to connect the ink to the microcontroller. This can be done by means of wires or even conductive tape.

By connecting the microcontroller to a computer and using any music production program or synthesizer, it is possible to start testing the interfaces.

Follow below our steps to start hearing our art pieces:

### Installation:
This section covers the software components of our project. The main branch contains all essential project files, including a dedicated folder for automation scripts. The Startup folder includes a script that automates remote handling via Automator on a MacBook. Additionally, the file "Compiler_Errors.h" is responsible for error management, while "DialogSchliesser.scpt" ensures smooth software operation when switching paper sheets.

For the music program, we use Ableton, where all pre-selected sounds are stored in the file "heartheart.als".

For the Arduino setup, you should use the Arduino IDE to open the "midi.ino" file and upload it then to the touch board.

To start the project, connect the touch board to a computer and power it on. Running the Automator script will initiate the automation process. Once Ableton is opened on the computer, the script will automatically load the correct Ableton file. After setup, placing a paper sheet on the device (prototype) will activate the system, making it ready for interaction.

You are ready to hear the art :)

This project was made by C. Plascencia (Product Design Student at KHB), M. Oprisiu and F. Azham (Computer Science Students at FUB).
