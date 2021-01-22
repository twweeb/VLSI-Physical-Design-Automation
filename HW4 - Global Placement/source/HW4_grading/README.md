# Homework 4 Grading Script
We will use this script to judge your program.
**Please make sure your program can be executed by this script.**

#
## Notice
Please add following code in your main.cpp before "return 0".
```
std::cerr << "HW4-result: " << bLegal << " " << fixed << placement.computeHpwl() << std::endl;
```

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
HW4_grading/student/${your_student_id}/CS6135_HW4_${your_student_id}.tar.gz
```
e.g.:
```
HW4_grading/student/109062501/CS6135_HW4_109062501.tar.gz
```
### Notice:
Do not put your original directory here because it will remove all directory before unzipping the compressed file.

#
## Working Flow
**Step1:**
Go into directory "HW4_grading/" and change the permission of "HW4_grading.sh".
```
$ chmod 744 HW4_grading.sh
```

**Step2:**
Run "HW4_grading.sh".
```
$ bash HW4_grading.sh
```

**Step3:**
Check your output.
* If status is "success", it means your program is finish in time and your output is correct. e.g.:
```
grading on 109062501:
  testcase |        overflow |      wirelength |    runtime | status
     ibm01 |               0 |       327525519 |       7.04 | success
     ibm05 |               0 |        28747584 |      38.92 | success
     ibm09 |               0 |      2674124033 |      54.64 | success
```
* If status is not "success", it means your program fails in this case. e.g.:
```
grading on 109062501:
  testcase |        overflow |      wirelength |    runtime | status
     ibm01 |     1277.720129 |       212729459 |       2.53 | ibm01 has overflow.
     ibm05 |            fail |            fail |        TLE | ibm05 failed.
     ibm09 |     1291.273997 |      1640571404 |      17.74 | ibm09 has overflow.
```