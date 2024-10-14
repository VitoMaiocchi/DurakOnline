## Table of Contents

1. [Git](#git)
   - Cloning the Repository
   - Before Starting Work: Pull Latest Changes
   - Creating a Branch for Your Work
   - Making Changes and Committing
   - Pushing Your Changes
   - Creating a Merge Request
   - Handling Merge Conflicts
   - Final Steps After Merging
2. [Durak](#durak)
   - Description
   - Rules
   - SRS Durak project first deadline
   - SDS Durak project second deadline
   - ToDo

---


# Git
<details> <summary>Click to expand Git section</summary>

To ensure smooth collaboration and avoid any repository mishaps, here's a clear Git workflow guide for our Durak project. Everyone works on their local machine, and we will use branches to isolate changes and avoid conflicts.

#### 1. **Cloning the Repository**

When you first start working on the project, you need to clone the repository to your local machine.

```bash
git clone https://gitlab.ethz.ch/durak/durak.git
cd durak
```

This will create a local copy of the repository. Make sure you navigate into the repository folder to start working.

---

#### 2. **Before Starting Work: Pull Latest Changes**

Before you start working each time, always pull the latest changes from the remote repository. This ensures your local copy is up to date with any changes made by others.

```bash
git pull origin main
```
This will fetch and merge any new changes into your local `main` branch.

---
> **Note:** While you can just use `git pull`, it defaults to pulling from the current branch's configured remote (usually `origin/main`). However, explicitly specifying `origin main` ensures you're pulling from the correct remote (`origin`) and branch (`main`). This is especially important when working with multiple remotes or branches, as it avoids accidental pulls from the wrong location.


---

#### 3. **Creating a Branch for Your Work**

Never work directly on the `main` branch. Instead, create a new branch for each feature or fix you're working on. This isolates your work from others.

```bash
git checkout -b feature/your-feature-name
```

Replace `your-feature-name` with a meaningful name related to your task, like `game-ui` or `bugfix-trump-rules`.

---

#### 4. **Making Changes and Committing**

Now, you're ready to make changes to the code. Once you're happy with your changes, stage the files and commit them.

1. Stage the files you've changed:

```bash
git add .
```

2. Commit your changes with a descriptive message:

```bash
git commit -m "Add new feature for card dealing logic"
```

Make sure your commit message is clear about what the changes are.

---

#### 5. **Pushing Your Changes**

After committing your changes, you need to push your branch to the remote repository.

```bash
git push origin feature/your-feature-name
```

This will upload your branch to the remote repository, making it available for others to review.

---
> **Note:** `origin` refers to the default name of the remote repository where your code is stored. It points to the repository URL you cloned from. We use `origin` to specify that we want to push or pull changes to/from this specific remote repository. It's important because it ensures you're working with the correct remote repository.
---



#### 6. **Creating a Merge Request**

Once your feature or fix is complete, create a Merge Request (MR) on GitLab to merge your branch into the `main` branch.

1. Go to the GitLab repository page.
2. Navigate to "Merge Requests."
3. Select "New Merge Request" and choose your feature branch as the source and `main` as the target.
4. Fill out the description and submit the MR.

Someone from the team will review your changes before they are merged into `main`.

---

#### 7. **Handling Merge Conflicts**

If there are merge conflicts when merging your branch into `main`, Git will prompt you to resolve them. You can see which files have conflicts with:

```bash
git status
```

After resolving the conflicts in the indicated files:

1. Stage the resolved files:
```bash
git add <filename>
```

2. Continue the merge process:

```bash
git commit
```
Finally, push the changes again:
```bash
git push origin feature/your-feature-name
```
---

#### 8. **Final Steps After Merging**

Once your changes are merged into the `main` branch, don't forget to update your local `main` branch to keep it in sync.

```bash
git checkout main git pull origin main
```

You can then delete your feature branch both locally and on the remote repository if you don't need it anymore.

- Delete the branch locally:

```bash
git branch -d feature/your-feature-name
```

- Delete the branch remotely:

```bash
git push origin --delete feature/your-feature-name
```
---

By following this workflow, we can collaborate effectively without running into issues like merge conflicts or overwriting each other's work.

</details>


# Durak
<details> <summary>Click to expand Durak section</summary>

## Description
Implementation of the game Durak in the curriculum of Software Engineering.
The rules might differ to the ones on [wikipedia](https://en.wikipedia.org/wiki/Durak).

## Rules 

<details> <summary>Click to expand Rules section</summary>

- **There is no winner, only one loser, the durak**
- You can only beat a card with another card of the same sign or trump.
- A trump card of any rank beats any non-trump card
- The ranking of the cards is as follows (ascending) **'2, 3, 4, 5, 6, 7, 8, 9, 10, J, Q, K, A.'**
- The game is counter-clockwise
- A battle is one turn
- An attack is a card that has been laid for the next player to defend
- A battle can consit of 6 small attacks (6 cards that are laid on the field).
- Only the cards that are already on the battle field can be added as attacks (the numbers must match).

##### Edge cases 
- The very first battle can only be a maximum of 5 attacks.
- The very first card cannot be passed on.
- 
</details>

## SRS Durak project first deadline 11.10

_link for the word document_: [word document](https://ethz-my.sharepoint.com/:w:/r/personal/dpoluyanov_ethz_ch/Documents/srs_durak.docx?d=w9ebff62a57a04528947512cd6dfb8ffc&csf=1&web=1&e=wYfiOv)

## SDS Durak project second deadline 25.10
_link for the word document_: [word document](https://ethz-my.sharepoint.com/:w:/r/personal/dpoluyanov_ethz_ch/Documents/sds_durak.docx?d=wee70dd9c8b994d2da4935af7b89b8c00&csf=1&web=1&e=7NLF7t)

_link for the instructions_: [Software Design](https://lec.inf.ethz.ch/se/2024_AS/project/book/phase2.html)

**_Hint:_** check out the lecture document for theory, especially the slides for Modeling and Specification: [lecture page](https://lec.inf.ethz.ch/se/2024_AS/)
### ToDO:

#### 1. Static modeling 
##### 1.1 package common
- classes: card, hand, player, discard pile, draw pile, game state, client request, server response
- diagram for classes 
##### 1.2 package server
- classes: server network manager, player manager, game instance, game instance manager, request handler
- diagram for classes
#### 2. Sequence diagram
##### 2.1 Sequence join game
##### 2.2 sequence play card
##### 2.3 sequence end of round

#### 3 Interface modeling
##### 3.1 Interface client server
- requests: join game, start game, play card, pass on, draw card, pick up cards(fold), req response, full state message



## Server team


## Client team



</details>








## Integrate with your tools

- [ ] [Set up project integrations](https://gitlab.ethz.ch/durak/durak/-/settings/integrations)

## Collaborate with your team

- [ ] [Invite team members and collaborators](https://docs.gitlab.com/ee/user/project/members/)
- [ ] [Create a new merge request](https://docs.gitlab.com/ee/user/project/merge_requests/creating_merge_requests.html)
- [ ] [Automatically close issues from merge requests](https://docs.gitlab.com/ee/user/project/issues/managing_issues.html#closing-issues-automatically)
- [ ] [Enable merge request approvals](https://docs.gitlab.com/ee/user/project/merge_requests/approvals/)
- [ ] [Set auto-merge](https://docs.gitlab.com/ee/user/project/merge_requests/merge_when_pipeline_succeeds.html)

## Test and Deploy

Use the built-in continuous integration in GitLab.

- [ ] [Get started with GitLab CI/CD](https://docs.gitlab.com/ee/ci/quick_start/index.html)
- [ ] [Analyze your code for known vulnerabilities with Static Application Security Testing (SAST)](https://docs.gitlab.com/ee/user/application_security/sast/)
- [ ] [Deploy to Kubernetes, Amazon EC2, or Amazon ECS using Auto Deploy](https://docs.gitlab.com/ee/topics/autodevops/requirements.html)
- [ ] [Use pull-based deployments for improved Kubernetes management](https://docs.gitlab.com/ee/user/clusters/agent/)
- [ ] [Set up protected environments](https://docs.gitlab.com/ee/ci/environments/protected_environments.html)

***

# Editing this README

When you're ready to make this README your own, just edit this file and use the handy template below (or feel free to structure it however you want - this is just a starting point!). Thanks to [makeareadme.com](https://www.makeareadme.com/) for this template.



