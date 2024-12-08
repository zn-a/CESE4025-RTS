# Assignment B: The Synthesizer Project

## Introduction

In this assignment, you will start with a fully-written code of a synthesizer. However, the synthesizer does not work as expected. The previous programmer didn't learn about real-time systems and is inefficiently organizing task execution in an infinite loop, a *superloop* in the embedded jargon. Your task is to fix and improve the code, such that the synthesizer works correctly, producing clear sound, without any clipping or clicking.

> Assignment B does not work with the latest Zephyr version (v4.0.0) due to a bug in the audio driver. Thus you are required to rollback to Version 3.7.1 before you can start working with Assignment-B. A guide to downgrade your Zephyr version is provided [here](https://cese.ewi.tudelft.nl/real-time-systems/assignment_b/reverting_zephyr.html).

## Practical information

***Deadline: January 18th 2025 23:59 (week 8)***

The assignment is submitted by pushing to the `main` branch of your repository. The last commit before the deadline will be considered your submission.

The report should be submitted as a `.pdf`, in the root of the `main` branch your GitLab repository. Use a maximum of **10 pages** for your report (A4, standard font size,  single-column, including images). The filename should be formatted as: `assignment_b_report_group_X.pdf` where `X` is your group number. Inside the report you should specify your names and student numbers. The template can be found [here](https://cese.ewi.tudelft.nl/real-time-systems/templates/assignment-b-template.zip).

> Make sure you **follow all the guidelines** mentioned in the report template.

## Assignment Instructions

The assignment instructions, a description of the codebase and other relevant reading material can be found on the [course website](https://cese.ewi.tudelft.nl/real-time-systems/assignment-b-intro.html)
