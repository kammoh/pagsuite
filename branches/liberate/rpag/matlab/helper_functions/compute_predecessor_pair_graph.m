%clear
%clc
%W = [17,27];
%W = [17175,589];
%W = [11,43,65];
%W = [177,499];
%W = [19290123283179,6543210987654321];
%W = [11280171,6910239];
%W = [4552139,11508221];
%W = [13342037,11508221];
function [predecessor_pairs] = compute_predecessor_pair_graph(W,ad_max)
complete = false; %test all cases incl. redundant (for test only!)

global verbose;

if ~exist('ad_max')
  ad_max=-1;
  for i=1:length(W)
    ad = adder_depth(W(i));
    if ad > ad_max
      ad_max = ad;
    end
  end
  ad_max=ad_max-1; %ad max of predecessor set must be one less

  if verbose >= 6
    disp(['maximum adder depth: ',num2str(ad_max)]);
  end
end

l_max = ceil(log2(max(W)))+1;

if verbose >= 6
  disp(['l_max=',num2str(l_max)]);
end

predecessor_pairs = [];
if complete
  predecessor_pairs_redundant = [];
end

for i=1:length(W)
  for j=i+1:length(W)
    w1 = W(i);
    w2 = W(j);
    
    for k=1:2
      if verbose >= 3
        disp(['evaluating pair (',num2str(w1),',',num2str(w2),')']);
      end
      
%      disp('case i:');
      l21=0; l22=0; r1=0; r2=0;
      for s12=0:1
        for s22=0:1
          for l11=1:l_max
            for l12=1:l_max
              [p1,p2] = evaluate_usefull_predecessor_pairs(w1,w2,l_max,ad_max,l11,l12,l21,l22,r1,r2,s12,s22);
              if p1>0
                disp(['case i: p1=',num2str(p1),', p2=',num2str(p2)]);
                predecessor_pairs = [predecessor_pairs;[p1,p2]];
              end
            end
          end
        end
      end

%      disp('case ii:');
      l21=0; l12=0; r1=0; r2=0;
      for s12=0:1
        for s22=0:1
          for l11=1:l_max
            for l22=1:l_max
              [p1,p2] = evaluate_usefull_predecessor_pairs(w1,w2,l_max,ad_max,l11,l12,l21,l22,r1,r2,s12,s22);
              if p1>0
                disp(['case ii: p1=',num2str(p1),', p2=',num2str(p2)]);
                predecessor_pairs = [predecessor_pairs;[p1,p2]];
              end
            end
          end
        end
      end

%      disp('case iii:');
      l21=0; l12=0; l22=0; r1=0; 
      for s12=0:1
        for s22=0:1
          for l11=1:l_max
            for r2=1:l_max
              [p1,p2] = evaluate_usefull_predecessor_pairs(w1,w2,l_max,ad_max,l11,l12,l21,l22,r1,r2,s12,s22);
              if p1>0
                disp(['case iii: p1=',num2str(p1),', p2=',num2str(p2)]);
                predecessor_pairs = [predecessor_pairs;[p1,p2]];
              end
            end
          end
        end
      end

%      disp('case iv:');
      l11=0; l21=0; l12=0; l22=0;  
      for s12=0:1
        for s22=0:1
          for r1=1:l_max
            for r2=1:l_max
              [p1,p2] = evaluate_usefull_predecessor_pairs(w1,w2,l_max,ad_max,l11,l12,l21,l22,r1,r2,s12,s22);
              if p1>0
                disp(['case iv: p1=',num2str(p1),', p2=',num2str(p2)]);
                predecessor_pairs = [predecessor_pairs;[p1,p2]];
              end
            end
          end
        end
      end

      if complete
        %redundant predecessors (for test only!):
%        disp('case v:');
        l11=0; l22=0; r1=0; r2=0;
        for s12=0:1
          for s22=0:1
            for l21=1:l_max
              for l12=1:l_max
                [p1,p2] = evaluate_usefull_predecessor_pairs(w1,w2,l_max,ad_max,l11,l12,l21,l22,r1,r2,s12,s22);
                if p1>0
                  predecessor_pairs_redundant = [predecessor_pairs_redundant;[p1,p2]];
                end
              end
            end
          end
        end

%        disp('case vi:');
        l11=0; l12=0; r1=0; r2=0;
        for s12=0:1
          for s22=0:1
            for l21=1:l_max
              for l22=1:l_max
                [p1,p2] = evaluate_usefull_predecessor_pairs(w1,w2,l_max,ad_max,l11,l12,l21,l22,r1,r2,s12,s22);
                if p1>0
                  predecessor_pairs_redundant = [predecessor_pairs_redundant;[p1,p2]];
                end
              end
            end
          end
        end

%        disp('case vii:');
        l11=0; l12=0; l22=0; r1=0;
        for s12=0:1
          for s22=0:1
            for l21=1:l_max
              for r2=1:l_max
                [p1,p2] = evaluate_usefull_predecessor_pairs(w1,w2,l_max,ad_max,l11,l12,l21,l22,r1,r2,s12,s22);
                if p1>0
                  predecessor_pairs_redundant = [predecessor_pairs_redundant;[p1,p2]];
                end
              end
            end
          end
        end

%        disp('case viii:');
        l11=0; l21=0; l22=0; r2=0;
        for s12=0:1
          for s22=0:1
            for r1=1:l_max
              for l12=1:l_max
                [p1,p2] = evaluate_usefull_predecessor_pairs(w1,w2,l_max,ad_max,l11,l12,l21,l22,r1,r2,s12,s22);
                if p1>0
                  predecessor_pairs_redundant = [predecessor_pairs_redundant;[p1,p2]];
                end
              end
            end
          end
        end

%        disp('case iv:');
        l11=0; l12=0; l21=0; r2=0;
        for s12=0:1
          for s22=0:1
            for r1=1:l_max
              for l22=1:l_max
                [p1,p2] = evaluate_usefull_predecessor_pairs(w1,w2,l_max,ad_max,l11,l12,l21,l22,r1,r2,s12,s22);
                if p1>0
                  predecessor_pairs_redundant = [predecessor_pairs_redundant;[p1,p2]];
                end
              end
            end
          end
        end
      end
      
      w1 = W(j);
      w2 = W(i);
    end
    predecessor_pairs = unique(predecessor_pairs,'rows');
%    num2str(predecessor_pairs)

    if complete
      predecessor_pairs_redundant = unique(predecessor_pairs_redundant,'rows');
      num2str(predecessor_pairs_redundant)
      setdiff(predecessor_pairs_redundant,predecessor_pairs,'rows')
      assert(length(setdiff(predecessor_pairs_redundant,predecessor_pairs,'rows')) == 0)
    end
  end
end
