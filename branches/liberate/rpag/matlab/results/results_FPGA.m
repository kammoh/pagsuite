addpath('../helper_functions');
addpath('../../../pipelined_mcm_opt/matlab/common_algorithms');

%benchmark_set = {'MIRZAEI10_6','MIRZAEI10_10','MIRZAEI10_13','MIRZAEI10_20','MIRZAEI10_28','MIRZAEI10_41','MIRZAEI10_61','MIRZAEI10_119','MIRZAEI10_151'};
benchmark_set = {'MIRZAEI10_6'};

%results after 50 runs:
PAG_fpga = containers.Map();

PAG_fpga('MIRZAEI10_6')=[5 1 1 0 0 1 0 2;9 1 1 0 0 1 0 3;15 1 1 0 4 -1 0 0;1 2 9 1 -2 -5 1 -2;625 2 5 1 7 -15 1 0;2551 2 5 1 9 -9 1 0;1 3 1 2 0 0 2 0;7567 3 1 2 13 -625 2 0;10203 3 2551 2 2 -1 2 0;7567 3 7567 3 0 0 3 0;20406 3 10203 3 1 0 3 0;32768 3 1 3 15 0 3 0];
PAG_fpga('MIRZAEI10_10')=[1 1 1 0 0 0 0 0;3 1 1 0 0 1 0 1;9 1 1 0 0 1 0 3;1 2 1 1 0 0 1 0;7 2 1 1 3 -1 1 0;25 2 3 1 3 1 1 0;189 2 3 1 6 -3 1 0;503 2 1 1 9 -9 1 0;1 3 1 2 0 0 2 0;505 3 1 2 9 -7 2 0;6589 3 189 2 0 25 2 8;7859 3 503 2 4 -189 2 0;12801 3 25 2 9 1 2 0;2020 3 505 3 2 0 3 0;6589 3 6589 3 0 0 3 0;15718 3 7859 3 1 0 3 0;25602 3 12801 3 1 0 3 0;32768 3 1 3 15 0 3 0];
PAG_fpga('MIRZAEI10_13')=[3 1 1 0 0 1 0 1;5 1 1 0 0 1 0 2;2049 1 1 0 0 1 0 11;1 2 5 1 -3 3 1 -3;3 2 3 1 0 0 1 0;83 2 5 1 4 3 1 0;189 2 3 1 6 -3 1 0;315 2 5 1 6 -5 1 0;14343 2 2049 1 3 -2049 1 0;1 3 1 2 0 0 2 0;259 3 3 2 0 1 2 8;387 3 3 2 0 3 2 7;2603 3 315 2 3 83 2 0;2659 3 83 2 5 3 2 0;12097 3 189 2 6 1 2 0;14727 3 14343 2 0 3 2 7;259 3 259 3 0 0 3 0;1548 3 387 3 2 0 3 0;5206 3 2603 3 1 0 3 0;12097 3 12097 3 0 0 3 0;21272 3 2659 3 3 0 3 0;29454 3 14727 3 1 0 3 0;32768 3 1 3 15 0 3 0];
PAG_fpga('MIRZAEI10_20')=[1 1 1 0 0 0 0 0;3 1 1 0 0 1 0 1;5 1 1 0 0 1 0 2;4095 1 1 0 12 -1 0 0;1 2 1 1 0 0 1 0;43 2 5 1 3 3 1 0;515 2 3 1 0 1 1 9;3075 2 3 1 0 3 1 10;4479 2 4095 1 0 3 1 7;1 3 1 2 0 0 2 0;3 3 1 2 0 1 2 1;15 3 1 2 4 -1 2 0;343 3 43 2 3 -1 2 0;687 3 43 2 4 -1 2 0;1031 3 515 2 1 1 2 0;1203 3 515 2 0 43 2 4;13405 3 515 2 5 -3075 2 0;17873 3 4479 2 2 -43 2 0;6 3 3 3 1 0 3 0;60 3 15 3 2 0 3 0;343 3 343 3 0 0 3 0;1374 3 687 3 1 0 3 0;4124 3 1031 3 2 0 3 0;9624 3 1203 3 3 0 3 0;17873 3 17873 3 0 0 3 0;26810 3 13405 3 1 0 3 0;32768 3 1 3 15 0 3 0];
PAG_fpga('MIRZAEI10_28')=[1 1 1 0 0 0 0 0;5 1 1 0 0 1 0 2;129 1 1 0 0 1 0 7;67 2 129 1 -1 5 1 -1;1 2 1 1 0 0 1 0;41 2 5 1 3 1 1 0;13 2 5 1 0 1 1 3;69 2 5 1 0 1 1 6;4133 2 129 1 5 5 1 0;1 3 1 2 0 0 2 0;37 3 13 2 3 -67 2 0;61 3 1 2 7 -67 2 0;175 3 67 2 1 41 2 0;297 3 41 2 0 1 2 8;309 3 67 2 2 41 2 0;397 3 69 2 0 41 2 3;469 3 67 2 3 -67 2 0;643 3 41 2 4 -13 2 0;831 3 13 2 6 -1 2 0;2077 3 67 2 5 -67 2 0;2209 3 69 2 5 1 2 0;4341 3 4133 2 0 13 2 4;4443 3 67 2 7 -4133 2 0;122 3 61 3 1 0 3 0;175 3 175 3 0 0 3 0;594 3 297 3 1 0 3 0;831 3 831 3 0 0 3 0;1184 3 37 3 5 0 3 0;2209 3 2209 3 0 0 3 0;2472 3 309 3 3 0 3 0;2572 3 643 3 2 0 3 0;3176 3 397 3 3 0 3 0;4154 3 2077 3 1 0 3 0;4443 3 4443 3 0 0 3 0;7504 3 469 3 4 0 3 0;8682 3 4341 3 1 0 3 0;32768 3 1 3 15 0 3 0];
PAG_fpga('MIRZAEI10_41')=[3 1 1 0 0 1 0 1;5 1 1 0 0 1 0 2;17 1 1 0 0 1 0 4;1 2 5 1 -3 3 1 -3;27 2 3 1 0 3 1 3;37 2 5 1 3 -3 1 0;71 2 17 1 2 3 1 0;45 2 3 1 4 -3 1 0;23 2 5 1 2 3 1 0;91 2 3 1 5 -5 1 0;113 2 17 1 0 3 1 5;1 3 1 2 0 0 2 0;15 3 1 2 4 -1 2 0;25 3 27 2 0 -1 2 1;31 3 1 2 5 -1 2 0;33 3 1 2 0 1 2 5;103 3 71 2 0 1 2 5;135 3 71 2 0 1 2 6;147 3 37 2 2 -1 2 0;157 3 23 2 3 -27 2 0;215 3 27 2 3 -1 2 0;217 3 27 2 3 1 2 0;333 3 45 2 3 -27 2 0;649 3 45 2 4 -71 2 0;675 3 45 2 4 -45 2 0;1211 3 37 2 5 27 2 0;1463 3 45 2 5 23 2 0;1977 3 1 2 11 -71 2 0;2245 3 71 2 5 -27 2 0;2841 3 91 2 5 -71 2 0;5779 3 91 2 6 -45 2 0;19057 3 113 2 0 37 2 9;31 3 31 3 0 0 3 0;100 3 25 3 2 0 3 0;135 3 135 3 0 0 3 0;147 3 147 3 0 0 3 0;157 3 157 3 0 0 3 0;217 3 217 3 0 0 3 0;264 3 33 3 3 0 3 0;430 3 215 3 1 0 3 0;666 3 333 3 1 0 3 0;675 3 675 3 0 0 3 0;960 3 15 3 6 0 3 0;1298 3 649 3 1 0 3 0;1463 3 1463 3 0 0 3 0;1648 3 103 3 4 0 3 0;1977 3 1977 3 0 0 3 0;2245 3 2245 3 0 0 3 0;2422 3 1211 3 1 0 3 0;2841 3 2841 3 0 0 3 0;5779 3 5779 3 0 0 3 0;19057 3 19057 3 0 0 3 0;32768 3 1 3 15 0 3 0];
PAG_fpga('MIRZAEI10_61')=[1 1 1 0 0 0 0 0;9 1 1 0 0 1 0 3;3 1 1 0 0 1 0 1;15 2 1 1 4 -1 1 0;13 2 9 1 0 1 1 2;97 2 3 1 5 1 1 0;1143 2 9 1 7 -9 1 0;3 2 3 1 0 0 1 0;5 2 1 1 0 1 1 2;147 2 9 1 4 3 1 0;279 2 9 1 5 -9 1 0;1 3 15 2 -1 -13 2 -1;83 3 3 2 5 -13 2 0;91 3 13 2 3 -13 2 0;97 3 97 2 0 0 2 0;135 3 15 2 0 15 2 3;149 3 97 2 0 13 2 2;155 3 1143 2 -3 97 2 -3;165 3 5 2 0 5 2 5;171 3 147 2 0 3 2 3;193 3 13 2 4 -15 2 0;217 3 97 2 0 15 2 3;225 3 15 2 4 -15 2 0;417 3 97 2 0 5 2 6;573 3 147 2 2 -15 2 0;575 3 147 2 2 -13 2 0;637 3 5 2 7 -3 2 0;655 3 15 2 0 5 2 7;663 3 1143 2 0 -15 2 5;679 3 97 2 3 -97 2 0;763 3 97 2 3 -13 2 0;817 3 13 2 6 -15 2 0;823 3 1143 2 0 -5 2 6;903 3 1143 2 0 -15 2 4;975 3 15 2 0 15 2 6;1055 3 279 2 0 97 2 3;1091 3 1143 2 0 -13 2 2;1149 3 1143 2 0 3 2 1;1161 3 147 2 3 -15 2 0;1337 3 1143 2 0 97 2 1;3075 3 3 2 0 3 2 10;9207 3 279 2 0 279 2 5;83 3 83 3 0 0 3 0;91 3 91 3 0 0 3 0;97 3 97 3 0 0 3 0;155 3 155 3 0 0 3 0;270 3 135 3 1 0 3 0;298 3 149 3 1 0 3 0;386 3 193 3 1 0 3 0;417 3 417 3 0 0 3 0;434 3 217 3 1 0 3 0;573 3 573 3 0 0 3 0;575 3 575 3 0 0 3 0;655 3 655 3 0 0 3 0;660 3 165 3 2 0 3 0;663 3 663 3 0 0 3 0;679 3 679 3 0 0 3 0;763 3 763 3 0 0 3 0;817 3 817 3 0 0 3 0;903 3 903 3 0 0 3 0;975 3 975 3 0 0 3 0;1055 3 1055 3 0 0 3 0;1091 3 1091 3 0 0 3 0;1274 3 637 3 1 0 3 0;1800 3 225 3 3 0 3 0;2298 3 1149 3 1 0 3 0;2322 3 1161 3 1 0 3 0;2736 3 171 3 4 0 3 0;3075 3 3075 3 0 0 3 0;3292 3 823 3 2 0 3 0;5348 3 1337 3 2 0 3 0;18414 3 9207 3 1 0 3 0;32768 3 1 3 15 0 3 0];
PAG_fpga('MIRZAEI10_119')=[1 1 1 0 0 0 0 0;9 1 1 0 0 1 0 3;3 1 1 0 0 1 0 1;5 1 1 0 0 1 0 2;31 1 1 0 5 -1 0 0;1 2 1 1 0 0 1 0;45 2 9 1 0 9 1 2;9 2 9 1 0 0 1 0;43 2 5 1 3 3 1 0;21 2 5 1 2 1 1 0;51 2 3 1 0 3 1 4;55 2 1 1 6 -9 1 0;143 2 9 1 4 -1 1 0;253 2 1 1 8 -3 1 0;1981 2 31 1 6 -3 1 0;1 3 1 2 0 0 2 0;3 3 1 2 0 1 2 1;5 3 1 2 0 1 2 2;7 3 1 2 3 -1 2 0;9 3 9 2 0 0 2 0;27 3 43 2 0 -1 2 4;31 3 1 2 5 -1 2 0;37 3 45 2 0 -1 2 3;39 3 43 2 0 -1 2 2;43 3 43 2 0 0 2 0;55 3 55 2 0 0 2 0;61 3 45 2 0 1 2 4;77 3 45 2 0 1 2 5;91 3 45 2 1 1 2 0;93 3 21 2 2 9 2 0;95 3 43 2 1 9 2 0;99 3 45 2 1 9 2 0;109 3 45 2 0 1 2 6;133 3 45 2 1 43 2 0;163 3 253 2 0 -45 2 1;165 3 21 2 0 9 2 4;171 3 43 2 2 -1 2 0;173 3 45 2 0 1 2 7;177 3 21 2 3 9 2 0;219 3 55 2 2 -1 2 0;221 3 55 2 2 1 2 0;223 3 45 2 2 43 2 0;265 3 9 2 0 1 2 8;271 3 143 2 0 1 2 7;287 3 9 2 5 -1 2 0;305 3 45 2 3 -55 2 0;337 3 21 2 4 1 2 0;359 3 45 2 3 -1 2 0;363 3 51 2 3 -45 2 0;369 3 1 2 9 -143 2 0;467 3 1 2 9 -45 2 0;613 3 253 2 0 45 2 3;621 3 45 2 0 9 2 6;675 3 45 2 4 -45 2 0;693 3 21 2 0 21 2 5;763 3 45 2 4 43 2 0;867 3 51 2 0 51 2 4;1021 3 253 2 2 9 2 0;1045 3 21 2 0 1 2 10;1109 3 9 2 7 -43 2 0;1323 3 21 2 6 -21 2 0;1577 3 51 2 5 -55 2 0;1589 3 51 2 5 -43 2 0;2253 3 9 2 8 -51 2 0;2297 3 143 2 4 9 2 0;2761 3 43 2 6 9 2 0;3125 3 1981 2 0 143 2 3;3357 3 1981 2 0 43 2 5;18355 3 143 2 7 51 2 0;12 3 3 3 2 0 3 0;28 3 7 3 2 0 3 0;31 3 31 3 0 0 3 0;77 3 77 3 0 0 3 0;95 3 95 3 0 0 3 0;108 3 27 3 2 0 3 0;144 3 9 3 4 0 3 0;148 3 37 3 2 0 3 0;154 3 77 3 1 0 3 0;156 3 39 3 2 0 3 0;165 3 165 3 0 0 3 0;173 3 173 3 0 0 3 0;192 3 3 3 6 0 3 0;198 3 99 3 1 0 3 0;219 3 219 3 0 0 3 0;244 3 61 3 2 0 3 0;265 3 265 3 0 0 3 0;287 3 287 3 0 0 3 0;326 3 163 3 1 0 3 0;337 3 337 3 0 0 3 0;342 3 171 3 1 0 3 0;344 3 43 3 3 0 3 0;354 3 177 3 1 0 3 0;359 3 359 3 0 0 3 0;363 3 363 3 0 0 3 0;372 3 93 3 2 0 3 0;432 3 27 3 4 0 3 0;436 3 109 3 2 0 3 0;440 3 55 3 3 0 3 0;446 3 223 3 1 0 3 0;467 3 467 3 0 0 3 0;530 3 265 3 1 0 3 0;532 3 133 3 2 0 3 0;542 3 271 3 1 0 3 0;613 3 613 3 0 0 3 0;621 3 621 3 0 0 3 0;640 3 5 3 7 0 3 0;675 3 675 3 0 0 3 0;693 3 693 3 0 0 3 0;728 3 91 3 3 0 3 0;738 3 369 3 1 0 3 0;763 3 763 3 0 0 3 0;867 3 867 3 0 0 3 0;1021 3 1021 3 0 0 3 0;1045 3 1045 3 0 0 3 0;1109 3 1109 3 0 0 3 0;1220 3 305 3 2 0 3 0;1577 3 1577 3 0 0 3 0;1589 3 1589 3 0 0 3 0;1768 3 221 3 3 0 3 0;2253 3 2253 3 0 0 3 0;2297 3 2297 3 0 0 3 0;2761 3 2761 3 0 0 3 0;3125 3 3125 3 0 0 3 0;3357 3 3357 3 0 0 3 0;5292 3 1323 3 2 0 3 0;18355 3 18355 3 0 0 3 0;32768 3 1 3 15 0 3 0];
PAG_fpga('MIRZAEI10_151')=[1 1 1 0 0 0 0 0;3 1 1 0 0 1 0 1;33 1 1 0 0 1 0 5;41 2 33 1 0 1 1 3;5 2 1 1 0 1 1 2;191 2 3 1 6 -1 1 0;7 2 1 1 3 -1 1 0;3 2 3 1 0 0 1 0;9 2 1 1 0 1 1 3;267 2 33 1 3 3 1 0;15 2 1 1 4 -1 1 0;29 2 1 1 5 -3 1 0;1 3 41 2 0 -5 2 3;5 3 5 2 0 0 2 0;21 3 41 2 0 -5 2 2;23 3 41 2 -1 5 2 -1;25 3 41 2 -1 9 2 -1;27 3 191 2 0 -41 2 2;41 3 41 2 0 0 2 0;47 3 41 2 0 3 2 1;49 3 191 2 -2 5 2 -2;79 3 41 2 1 -3 2 0;81 3 41 2 0 5 2 3;97 3 41 2 0 7 2 3;105 3 7 2 4 -7 2 0;109 3 191 2 0 -41 2 1;113 3 41 2 0 9 2 3;119 3 5 2 5 -41 2 0;121 3 41 2 0 5 2 4;127 3 15 2 3 7 2 0;137 3 41 2 3 -191 2 0;139 3 9 2 4 -5 2 0;163 3 5 2 5 3 2 0;169 3 41 2 2 5 2 0;173 3 41 2 2 9 2 0;179 3 41 2 2 15 2 0;181 3 191 2 0 -5 2 1;195 3 7 2 5 -29 2 0;197 3 5 2 0 3 2 6;199 3 15 2 4 -41 2 0;201 3 41 2 0 5 2 5;205 3 41 2 0 41 2 2;211 3 191 2 0 5 2 2;215 3 191 2 0 3 2 3;217 3 7 2 5 -7 2 0;265 3 41 2 0 7 2 5;285 3 9 2 5 -3 2 0;291 3 5 2 6 -29 2 0;323 3 41 2 3 -5 2 0;327 3 7 2 0 5 2 6;331 3 41 2 3 3 2 0;333 3 41 2 3 5 2 0;343 3 3 2 7 -41 2 0;347 3 267 2 0 5 2 4;351 3 191 2 0 5 2 5;355 3 191 2 0 41 2 2;361 3 41 2 0 5 2 6;369 3 41 2 0 41 2 3;377 3 191 2 1 -5 2 0;383 3 191 2 0 3 2 6;397 3 191 2 1 15 2 0;407 3 7 2 6 -41 2 0;413 3 29 2 0 3 2 7;423 3 191 2 1 41 2 0;427 3 267 2 0 5 2 5;433 3 7 2 6 -15 2 0;479 3 191 2 0 9 2 5;535 3 9 2 6 -41 2 0;595 3 267 2 0 41 2 3;643 3 5 2 7 3 2 0;723 3 191 2 2 -41 2 0;735 3 191 2 2 -29 2 0;747 3 267 2 0 15 2 5;805 3 191 2 2 41 2 0;843 3 267 2 0 9 2 6;855 3 7 2 7 -41 2 0;1063 3 267 2 2 -5 2 0;1111 3 9 2 7 -41 2 0;1261 3 191 2 3 -267 2 0;1271 3 41 2 5 -41 2 0;1795 3 267 2 0 191 2 3;1797 3 7 2 8 5 2 0;4081 3 267 2 4 -191 2 0;158 3 79 3 1 0 3 0;181 3 181 3 0 0 3 0;194 3 97 3 1 0 3 0;199 3 199 3 0 0 3 0;200 3 25 3 3 0 3 0;210 3 105 3 1 0 3 0;211 3 211 3 0 0 3 0;226 3 113 3 1 0 3 0;238 3 119 3 1 0 3 0;242 3 121 3 1 0 3 0;265 3 265 3 0 0 3 0;285 3 285 3 0 0 3 0;291 3 291 3 0 0 3 0;320 3 5 3 6 0 3 0;323 3 323 3 0 0 3 0;324 3 81 3 2 0 3 0;326 3 163 3 1 0 3 0;327 3 327 3 0 0 3 0;328 3 41 3 3 0 3 0;331 3 331 3 0 0 3 0;336 3 21 3 4 0 3 0;338 3 169 3 1 0 3 0;343 3 343 3 0 0 3 0;346 3 173 3 1 0 3 0;347 3 347 3 0 0 3 0;351 3 351 3 0 0 3 0;355 3 355 3 0 0 3 0;358 3 179 3 1 0 3 0;361 3 361 3 0 0 3 0;368 3 23 3 4 0 3 0;369 3 369 3 0 0 3 0;376 3 47 3 3 0 3 0;377 3 377 3 0 0 3 0;383 3 383 3 0 0 3 0;390 3 195 3 1 0 3 0;392 3 49 3 3 0 3 0;394 3 197 3 1 0 3 0;397 3 397 3 0 0 3 0;402 3 201 3 1 0 3 0;407 3 407 3 0 0 3 0;410 3 205 3 1 0 3 0;413 3 413 3 0 0 3 0;420 3 105 3 2 0 3 0;423 3 423 3 0 0 3 0;427 3 427 3 0 0 3 0;430 3 215 3 1 0 3 0;432 3 27 3 4 0 3 0;433 3 433 3 0 0 3 0;434 3 217 3 1 0 3 0;436 3 109 3 2 0 3 0;479 3 479 3 0 0 3 0;508 3 127 3 2 0 3 0;535 3 535 3 0 0 3 0;595 3 595 3 0 0 3 0;666 3 333 3 1 0 3 0;735 3 735 3 0 0 3 0;747 3 747 3 0 0 3 0;805 3 805 3 0 0 3 0;843 3 843 3 0 0 3 0;958 3 479 3 1 0 3 0;1063 3 1063 3 0 0 3 0;1096 3 137 3 3 0 3 0;1261 3 1261 3 0 0 3 0;1271 3 1271 3 0 0 3 0;1494 3 747 3 1 0 3 0;1795 3 1795 3 0 0 3 0;1797 3 1797 3 0 0 3 0;2222 3 1111 3 1 0 3 0;2224 3 139 3 4 0 3 0;2892 3 723 3 2 0 3 0;4081 3 4081 3 0 0 3 0;6840 3 855 3 3 0 3 0;20576 3 643 3 5 0 3 0;32768 3 1 3 15 0 3 0];

keys = PAG_fpga.keys;
values = PAG_fpga.values;

%for i=1:length(PAG_mcm_min_ad)
for i=1:length(benchmark_set)
  plot_pipelined_realization_graph(cell2mat(values(i)),'name',['PAG_mcm_min_ad_',cell2mat(keys(i))]);
  pipelined_realization = PAG_fpga(benchmark_set{i});
  disp([benchmark_set{i},'=',mat2str(pipelined_realization)]);
  reg_ops = nnz(pipelined_realization(:,2)~=pipelined_realization(:,4));
  regs=nnz((pipelined_realization(:,1)==pipelined_realization(:,3)) & (pipelined_realization(:,2)~=pipelined_realization(:,4)));
  adds=nnz((pipelined_realization(:,1)~=pipelined_realization(:,3)) & (pipelined_realization(:,2)~=pipelined_realization(:,4)));
%  disp([benchmark_set{i},': ',num2str(adds),' registered adds, ',num2str(regs),' registers, ',num2str(reg_ops),' registered ops']);
end
