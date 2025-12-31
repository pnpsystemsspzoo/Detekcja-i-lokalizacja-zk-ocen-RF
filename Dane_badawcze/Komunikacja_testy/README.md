# APU application
This repository contains Linux application source files dedicated for the FPGA chip.
### Cloning
Use either http

    git clone http://192.168.10.3:82/pnpsystems/informatycy/sonda-fpga.git 
... or SSH connection ([how to setup SSH connection with your git account](https://docs.gitlab.com/ee/ssh/))

    git clone git@192.168.10.3:pnpsystems/informatycy/sonda-fpga.git
### Building
##### Automated builds
All builds are managed by Jenkins CI/CD automation server. There exist two separete routines here:
1) **Build only**<br>
Applies for branches prefixed by **_feature/_** e.i. any push to these branches will start pipeline with **_Jenkins_** build job.
2) **Build & deploy**<br>
Applies only for **_master_** branch e.i. any push to _master_ branch will result in starting Jenkins build+deploy job. Binary files will be deployed into the prototype FPGA testing device.<br>

###### Manual builds
If you need to trigger build manually **on remote server**. Please follow these steps:<br>
Login to the **_Kołtun_** server through SSH

    ssh <your-user-name>@192.168.16.2
Go to Petalinux build root directory

    cd /home/software/Device/APU
Copy your source files into **_sonda_** folder and build with command:

    ./compile.sh manual
If you want also for binary files to be deployed on testing device, run:

    ./deploy.sh
     
---
If you want to build sources **on your local machine** to check compilation before pushing to remote GitLab repository, it is advised to use **gcc 4.8.x** version compiler. Currently, there is no cmake project configuration file available, so you need to configure it by yourself.
### Working with repository
##### Master branch
Main repository branch is named **_master_**. This branch is set to be _protected_ and is used to build and deploy latest binaries to the testing device. It means any direct push to this branch **is not allowed**. Although, you may have access to **_master_** branch (if you are repository maintainer or owner) it is still not recommended to execute direct pushes if it's not mandatory.

##### Normal workflow branch
Every normal branch your are working with must be preceeded with **_feature/_** tag. This is the keyword trigger set for **_Jenkins_**
 i.e. every branch that starts with **_feature_** will be posted for automated build on the remote server. Accordingly, if you do not want to trigger automated builds create branch that do not contains **_feature_** tag in its name. Also, please note only **_master_** branch is allowed to deploy binaries into testing device. For normal wokflow branches <u>only build step will be executed</u>. If you want to deploy the code open merge request.
 
 ---
 **Example use case:**
<br>
You are beginning to working on the new functionality. Assuming you have already cloned repository.<br>
1) Open git bash at directory of your local repository
2) Checkout to master and pull latest changes from remote<br>
   
       git checkout master
       git pull

3) Create new branch where you will be working. Choose some descriptive name for it and preceed with **_feature/_**<br>

       git checkout -b feature/your_descriptive_branch_name

4) Make some changes in source files
5) Notice git you have made some changes in source files that should be tracked.<br>
If you want git to track all touched files, type:<br>

       git add -A
    ... or specify which files to track manually:<br>
       
       git add file1.cpp file2.h file3.txt
       
6) Create commit for applied changes<br>

       git commit -m "This is the description for this commit"
       
 7) Push changes to remote server:<br>
    For the first time there is no remote branch under newly created local branch. Need to set it with command:<br>
 
        git push --set-upstream origin feature/your_descriptive_branch_name<br>
        
    For next pushes just type:
    
        git push
   
8) After your new functionality being completed you can merge into master and close current working branch<br>

       git merge origin/master
       
   If there is no conflict you can push merge to remote<br>
   
       git push
       
   Now you should see prompt you can open merge request with specified link to it. Click on link and open new merge request. Wait for the pipeline to pass and if it's successfull click merge button or ask someone with permissions to do it.<br>

##### mock/* branches
These branches are used for building mock version. **Do not** merge master branch with this!<br>
Any push to the branch with prefix **_mock/_** will result in trigger Jenkins **build only** job of the mocked version.<br>
Usage is similar to **_feature/_** branches:<br>

1) Open git bash at directory of your local repository
2) Checkout to **_master-mock_** and pull latest changes from remote<br>
   
       git checkout master-mock
       git pull

3) Create new branch where you will be working. Choose some descriptive name for it and preceed with **_mock/_**<br>

       git checkout -b mock/your_descriptive_branch_name
       
4) Make some changes in source files
5) Notice git you have made some changes in source files that should be tracked.<br>
If you want git to track all touched files, type:<br>

       git add -A
    ... or specify which files to track manually:<br>
       
       git add file1.cpp file2.h file3.txt
       
6) Create commit for applied changes<br>

       git commit -m "This is the description for this commit"
       
 7) Push changes to remote server:<br>
    For the first time there is no remote branch under newly created local branch. Need to set it with command:<br>
 
        git push --set-upstream origin mock/your_descriptive_branch_name<br>
        
    For next pushes just type:
    
        git push
   
8) After your new functionality being completed you can merge into **_master-mock_** and close current working branch<br>

       git merge origin/master-mock
       
   If there is no conflict you can push merge to remote<br>
   
       git push
       
   Now you should see prompt you can open merge request with specified link to it. Click on link and open new merge request. **Set target branch to master-mock**. Wait for the pipeline to pass and if it's successfull click merge button or ask someone with permissions to do it.<br>


##### Master-mock branch
Branch to build and deploy probe mocks is called **_master-mock_** branch. Any push to this branch will result with **building and deploying** sources to the probe mocks. 
    
### Working with Pipelines

For each push to **_feature_** or **_master_** new **_Jenkins_** job will be invoked and can be seen from GitLab site as the pipeline. You can see all pipelines list [here](http://192.168.10.3:82/pnpsystems/informatycy/sonda-fpga/pipelines). Choose your pipeline by clicking on his name under the _Pipeline_ column. You will be redirected to pipeline overwiew site. There you can see all job stages for this pipeline. Click on any stage you want and you will be redirected to _Jenkins_ http service. From dashboard choose _Console Logs_ to see current job logs. There you can find about specific informations if any error occured in current pipeline.