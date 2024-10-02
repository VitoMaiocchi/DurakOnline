
# Git

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



# Durak


## Description
Implementation of the game Durak in the curriculum of Software Engineering.
The rules might differ to the ones on [wikipedia](https://en.wikipedia.org/wiki/Durak).

## Rules 
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

## SRS Durak project first deadline

_link for the word document_: [word document](https://ethz-my.sharepoint.com/:w:/r/personal/dpoluyanov_ethz_ch/Documents/srs_durak.docx?d=w9ebff62a57a04528947512cd6dfb8ffc&csf=1&web=1&e=wYfiOv)

### ToDO

#### 1. Title page and table of content 
whoever is first

#### 2. Overall description 
@twaldmann

#### 3. External interface and requirements 
@eholenstein
- User Interface
- Communication Interface 

#### 4. Systems requirements
@norutz
- 12 functional 

#### 5. System scenarios
@dpoluyanov
- 4 scenarios 


#### 6. System constrains
@twaldmann
- 4 non-functional

## Server team


## Client team












## Add your files

- [ ] [Create](https://docs.gitlab.com/ee/user/project/repository/web_editor.html#create-a-file) or [upload](https://docs.gitlab.com/ee/user/project/repository/web_editor.html#upload-a-file) files
- [ ] [Add files using the command line](https://docs.gitlab.com/ee/gitlab-basics/add-file.html#add-a-file-using-the-command-line) or push an existing Git repository with the following command:

```
cd existing_repo
git remote add origin https://gitlab.ethz.ch/durak/durak.git
git branch -M main
git push -uf origin main
```

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

## Suggestions for a good README

Every project is different, so consider which of these sections apply to yours. The sections used in the template are suggestions for most open source projects. Also keep in mind that while a README can be too long and detailed, too long is better than too short. If you think your README is too long, consider utilizing another form of documentation rather than cutting out information.

## Name
Choose a self-explaining name for your project.

## Description
Let people know what your project can do specifically. Provide context and add a link to any reference visitors might be unfamiliar with. A list of Features or a Background subsection can also be added here. If there are alternatives to your project, this is a good place to list differentiating factors.

## Badges
On some READMEs, you may see small images that convey metadata, such as whether or not all the tests are passing for the project. You can use Shields to add some to your README. Many services also have instructions for adding a badge.

## Visuals
Depending on what you are making, it can be a good idea to include screenshots or even a video (you'll frequently see GIFs rather than actual videos). Tools like ttygif can help, but check out Asciinema for a more sophisticated method.

## Installation
Within a particular ecosystem, there may be a common way of installing things, such as using Yarn, NuGet, or Homebrew. However, consider the possibility that whoever is reading your README is a novice and would like more guidance. Listing specific steps helps remove ambiguity and gets people to using your project as quickly as possible. If it only runs in a specific context like a particular programming language version or operating system or has dependencies that have to be installed manually, also add a Requirements subsection.

## Usage
Use examples liberally, and show the expected output if you can. It's helpful to have inline the smallest example of usage that you can demonstrate, while providing links to more sophisticated examples if they are too long to reasonably include in the README.

## Support
Tell people where they can go to for help. It can be any combination of an issue tracker, a chat room, an email address, etc.

## Roadmap
If you have ideas for releases in the future, it is a good idea to list them in the README.

## Contributing
State if you are open to contributions and what your requirements are for accepting them.

For people who want to make changes to your project, it's helpful to have some documentation on how to get started. Perhaps there is a script that they should run or some environment variables that they need to set. Make these steps explicit. These instructions could also be useful to your future self.

You can also document commands to lint the code or run tests. These steps help to ensure high code quality and reduce the likelihood that the changes inadvertently break something. Having instructions for running tests is especially helpful if it requires external setup, such as starting a Selenium server for testing in a browser.

## Authors and acknowledgment
Show your appreciation to those who have contributed to the project.

## License
For open source projects, say how it is licensed.

## Project status
If you have run out of energy or time for your project, put a note at the top of the README saying that development has slowed down or stopped completely. Someone may choose to fork your project or volunteer to step in as a maintainer or owner, allowing your project to keep going. You can also make an explicit request for maintainers.
