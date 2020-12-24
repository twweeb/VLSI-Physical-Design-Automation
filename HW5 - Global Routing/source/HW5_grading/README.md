# Homework 5 Grading Script
We will use this script to judge your program.
**Please make sure your program can be executed by this script.**

#
## Preparing
**Step1:**
Go into directory "student/" and generate a new directory with your student id.
```
$ cd student/
$ mkdir ${your_student_id}
$ cd ${your_student_id}/
```
e.g.:
```
$ cd student/
$ mkdir 109062501
$ cd 109062501/
```

**Step2:**
Put your compressed file in the directory which you just generate. The whole path is as follow: 
```
HW5_grading/student/${your_student_id}/CS6135_HW5_${your_student_id}.tar.gz
```
e.g.:
```
HW5_grading/student/109062501/CS6135_HW5_109062501.tar.gz
```
### Notice:
Do not put your original directory here because it will remove all directory before unzipping the compressed file.

#
## Working Flow
**Step1:**
Go into directory "HW5_grading/" and change the permission of "HW5_grading.sh".
```
$ chmod 744 HW5_grading.sh
```

**Step2:**
Run "HW5_grading.sh".
```
$ bash HW5_grading.sh
```

**Step3:**
Check your output.
* If status is "success", it means your program is finish in time and your output is correct. e.g.:
```
grading on 109062501:
  testcase |   overflow | wirelength |    runtime | status
     ibm01 |          0 |      62247 |       0.90 | success
     ibm04 |        196 |     161806 |     110.80 | success
```
* If status is not "success", it means your program fails in this case. e.g.:
```
grading on 109062501:
  testcase |   overflow | wirelength |    runtime | status
     ibm01 |       fail |       fail |       0.62 | ibm01 has floating wires.
     ibm04 |       fail |       fail |     108.06 | ibm04 has something error.
```