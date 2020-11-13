# Homework 2 Grading Script
We will use this script to judge your program.
**Please make sure your program can be executed by this script.**

#
## Preparing
**Step1:**
Go into directory "student/" and generate a new directory with your student id.
```
$ cd student/
$ mkdir {your_student_id}
$ cd {your_student_id}/
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
HW2_grading/student/{your_student_id}/CS6135_HW2_{your_student_id}.tar.gz
```
e.g.:
```
HW2_grading/student/109062501/CS6135_HW2_109062501.tar.gz
```
### Notice:
Do not put your original directory here because it will remove all directory before unzipping the compressed file.

#
## Working Flow
**Step1:**
Go into directory "HW2_grading/" and change the permission of "HW2_grading.sh".
```
$ chmod 744 HW2_grading.sh
```

**Step2:**
Run "HW2_grading.sh".
```
$ bash HW2_grading.sh
```

**Step3:**
Check your output.
* If status is "success", it means your program is finish in time and your output is correct. e.g.:
```
grading on 109062501:
  testcase |   cut size |    runtime | status
    pub2-1 |         20 |       0.01 | success
    pub2-2 |        482 |       0.04 | success
    pub2-3 |        605 |       1.62 | success
    pub2-4 |      45220 |       2.76 | success
    pub2-5 |     124677 |      21.19 | success
    hid2-1 |      75476 |       6.40 | success
    hid2-2 |      99967 |      19.22 | success
    hid2-3 |     139822 |      33.15 | success
```
* If status is not "success", it means your program failed in this case. e.g.:
```
grading on 109062501:
  testcase |   cut size |    runtime | status
    pub2-1 |       fail |        TLE | pub2-1 failed.
    pub2-2 |       fail |        TLE | pub2-2 failed.
    pub2-3 |       fail |        TLE | pub2-3 failed.
    pub2-4 |       fail |        TLE | pub2-4 failed.
    pub2-5 |       fail |        TLE | pub2-5 failed.
    hid2-1 |       fail |        TLE | hid2-1 failed.
    hid2-2 |       fail |        TLE | hid2-2 failed.
    hid2-3 |       fail |        TLE | hid2-3 failed.
```