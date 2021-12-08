# Assistance Bots

## Introduction 🏁

Automation has substantially improved the way computers can help us. One good example are bots, programs that do repeatitive, pre-defined tasks. A bot may, in instance, periodically check the price of a saved item and alert you when it goes below a predefined level, or to automatically save a copy of all the photos and videos where you are mentioned in a social network.

In this project you are asked to develop a notes-taking application and few bots to extend its functionalities.

## Description 📃

The application shall interact with the user through the standard input/output using a collection of commands (with their parameters).

Your program shall implement the following features:

- Create a new note.
  - Input: command `create`, followed by the note.
  - Output: `CREATED`.
- Search for notes with a given tag.
  - Input: command `tag`, followed by the tag name.
  - Output: a line with number of notes matching the search criteria, followed by the notes, one per line.
- Search for notes mentioning someone.
  - Input: command `mention`, followed by the name to look up.
  - Output: a line with number of notes matching the search criteria, followed by the notes, one per line.
- Show the agenda for a given date, it is, notes where the date appears.
  - Input: command `agenda`, followed by the date in either `yyyy/mm/dd` or `dd/mm/yyyy`.
  - Output: a line with number of notes matching the search criteria, followed by the notes, one per line.
- Prove the functionality of your own bot.
  - Input: command `bot`.
  - Output: the result of your bot.
- Respect user's privacy. If a note starts with a question mark `?` then bots should not receive that note because it is a private note.
- Bots shall be executed in parallel, no matter if previous bots have finished.
- If a command contains an invokation to a bot, you shall wait until all the notes have been processed by that bot before being able to answer it.
- Optionally you can implement a publisher/subscriber mechanism for extra points (see below).

### Input ⬅️

- Your application will receive commands through the standard input, one command per line.
- All commands and arguments are case sensitive. For example, if you try command `CreaTE`, it won't be valid.
- All commands will be valid, so do not expect ill-formed lines.
- Every note will have no more than 10 million characters.
- Every command ends with an EOL (end-of-line).
- Each test file will have no more than 100 notes.

For example, the following input will create several notes (one private), search for tag `tecnun`, list the notes on `2021/10/21`, and search for tag `course`:

```text
create This is my first sample note for #tecnun course. We started on 21/10/2021.
create ? This is a private note at #tecnun.
create Yeap, I have a good feeling about this group and the notes application they will develop #tecnun.
tag tecnun
date 2021/10/21
create This note is for 21/10/2021 but should not be listed because it is created after the command 'date' above was invoked.
tag course
```

### Output ➡️

Results for every command shall be done through the standard output.

For the example above:

```text
CREATED
CREATED
CREATED
2
This is my first sample note for #tecnun course. We started on 21/10/2021.
Yeap, I have a good feeling about this group and the notes application they will develop #tecnun.
1
This is my first sample note for #tecnun course. We started on 21/10/2021.
CREATED
0
```

Note that the private note was not listed in the output of commands.

### Bots 🤖

When a non-private note is created, bots are notified and executed in parallel (each bot shall run in a separate thread). Therefore, the user may introduce several notes while bots are still processing them.

To meet the previous requirement take into consideration:

- Bots shall not print information during their execution, only when the associated command is invoked.
- Every time a note is created, you have to create a new thread per bot to handle it. It means that several bots of the same type may be running at the same time, so take the necessary precautions to prevent race conditions.
- Once a bot finishes processing a note, it is destroyed.
- When a `create` command is received, the application shall create the note and invoke bots, but it shall not wait for them to finish before processing the next command.
- On the other hand, if the command is a bot command (`tag`, `date`, `agenda`, `bot`) then the application shall wait that all the bots of the requested type have finished before processing the command and show the result.
- Do not assume your bots will be fast enough to process a note before next one arrives. To test it you can try delaying the execution of each both a random amount of milliseconds (between 100ms and 500ms, for example). You can find a sample code for this [here](https://github.com/cbuchart/operating_systems_lectures/blob/master/src/ThreadOrTrick/README.md#sleeping-a-thread) and [here](https://github.com/cbuchart/operating_systems_lectures/blob/master/src/ThreadOrTrick/README.md#generating-random-numbers).

#### Classification 🏷

- Create classifications based on tags (`#`) and mentions (`@`).
- Text associated with a tag/mention is case sensitive (`#tecnun` and `#TECNUN` are different tags).
- Text associated with a tag/mention are composed of alphanumeric characters only (`A`-`Z`, `a`-`z`, `0`-`9`), with a minimum length of 1 character. Any other character will mark the end of the tag (so `#tecnun-2021` represents tag `tecnun`).
- Tags/mentions have no size limit but the size of the note.

#### Agenda 🗓

- Date formats allowed include: `yyyy/mm/dd`, `dd/mm/yyyy`, where `yyyy` is a four digits year, `mm` two digits month, `dd` two digits day. Any date not matching this criteria shall be considered a non-date.

#### Your own bot 🆒

Be creative! 🎨 The only limitations you have are:

- Independently of what you bot does, its command name will be `bot`, with no arguments.
- Shall no show any information during its execution, only when explicitly called.
- Cannot duplicate or erase the note, only modify it.

### Optional: publisher-subscriber 📬

Using a publisher-subscriber to decouple the bots from the specifics of the notes application is non-mandatory for the project, but will have a generous compensation is done correctly 🎁.

The [publisher-subscriber pattern](https://en.wikipedia.org/wiki/Publish%E2%80%93subscribe_pattern) (pub/sub) is a messaging solution where senders (publishers) do not send messages directly to the receivers, but instead categorize them. Receivers (subscribers) express interest in one or more categories. Neither publishers or subscribers know anything about the others up to a point that there may be publishers without subscribers and viceversa.

In a very simple way, a _publisher_ is a class that offers a `publish(topic, message)` method, and a _subscriber_ is a class that offers a `subscribe(topic)` and `on_received(message)` methods. Both have to be connected to the same _board_. Each time a message is published on a topic, the board checks for all the objects subscribed to that topic and calls their `on_received`, passing the message as an argument.

The workshop scheduled for November 12th (Friday, both classes) will cover the implementation details of this mechanism.

## Language and compiler 🖥

- It shall be developed in C++ (up to C++17).
- No third-party libraries are allowed, only the C++ standard libraries.
- You can use the IDE and compiler you want as long as you specify which one you used and the compilation command line, solution file or makefile necessary to build the project.
- You can develop for either x86 or x64 platforms. No other architectures, like ARM, are allowed.

## Rules 📏

- Source code and build instructions shall be sent through ADI before December 1st, 2021 at 23:59:59 UTC+1. You can modify your submission until that deadline, but no submissions will be accepted after that. Only one member of the team should upload the project.
- The submission must be a zip file containing the source code, project files, and a document listing the members of the group, build instructions and a brief explanation of your bot. Include please a brief description of your bot and what it supposed to do. Do not include binaries, nor temporary build files.
- Questions about the project will be accepted up to 48h before the deadline. Any last minute question may not be answered.
- There will be a face-to-face session dedicated to questions about the project, scheduled on November 11th, 2021.
- The application will be tested against a group of input files, some of them hidden to you. You will receive some sample input/output pairs for testing, but take in mind in may not cover all the possible cases, and that final input files may stress your application with large notes, many tags/mentions/dates, etc.
- Your bot will be tested manually.
- Your application can be 32 or 64 bits, but test both to check which one is more efficient.
- Sample execution line: `notes.exe < test_1.txt > test_1.out`
- Test machine will be a MacBook Pro 2019 (6-core Intel i7, 32 GB RAM), running macOS Big Sur or Monterey.

## Grading 🥇

- The project scores for 50% of Operating Systems part (it is, 25% of the Course).
- The output for each will be compared with the correct answer, if different it will be marked as invalid. Please pay attention to extra spaces and new-lines.
- Final score will be computed based on the number of correct input cases solved.
- The project will not be graded at all if it does not compile or if it is received out of date.
- Code elegance is particularly important: unorganized, not indented code will suppose a penalization of 10%.
- Unsolved race conditions will suppose 50% of penalization since they may produce an incorrect output.
- Missing multi-thread will have an impact of 60% of penalization.
- Implementing correctly the pub/sub mechanism will receive a bonus of 50% over the project's value. Overflow will be derived to the final exam 😁🎉.
- Cheating, plagiarism, or any other form of copy of other's work is a serious violation. It will be sanctioned with the absolute rejection of the project. If it involves two or more projects, all of them will be rejected, independently of who copied who. It also applies to the optional part. Nevertheless, and as explained several times, this is not a competition and we encourage collaboration; just avoid doing other ones job.
- Submit only the source code, instructions and project files, do not submit sample nor binary files. Doing so will be penalized with a 5% off.
- Penalizations also applies to the extra-part.

## Tips 💡

- Do not try to solve the whole project at one, use an incremental approach for development instead. For example:
  - Create an application that can create notes. Add a temporary command just to print the list of notes.
  - Classify notes between private and non-private notes.
  - Add one bot with no threads.
  - Add the missing bots.
  - Add the multi-threading.
  - The pub/sub mechanism is not mandatory and will probably take you time, so you can ensure you have everything else working before implementing it.
- Discuss with your team the possible race conditions.
- Be aware that bots are independant of each other, so the job done by a bot has no effects on the rest.
- Be sure to save a copy without the pub/sub so you can deliver it in the case you cannot get it working.
- When pub/sub is ready, use two topics, one for private and one for non-private notes, then subscribe all your bots to the non-private.

## Solution

You can find one solution to this problem [here](https://github.com/cbuchart/tecnun_assistance_bots).

