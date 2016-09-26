function coeff = get_benchmark_coefficients(benchmark_name)
  switch benchmark_name
    case 'test1'
      coeff = [44,130,172];
    case 'test2'
      coeff = [480,512,846,1020];
    case 'test3'
      coeff = [171];
    case 'test4'
      coeff = [43691];
    case 'test5'
      coeff = [3006477107];
    case 'test6'
      coeff = 6378789;
    case 'test7'
      coeff = 1675;
    case 'test8'
      coeff = [7567 20406];
    case 'sqrt2_double'
      coeff = [932726022577638];
    case '20bit'
      coeff = [165269 1017740 1003662 508953 839155 148779 442249 960218 830690 1006101];
    otherwise
      conv_matrix = benchmarkfilter2d(benchmark_name);
      if conv_matrix < 0
        coeff = get_reconf_mirzaei_benchmark_coeffs(benchmark_name);
      else
        coeff = unique(fundamental(abs(unique(conv_matrix)')));
        coeff = setdiff(coeff,[0]);
      end
  end