%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    PhD Project of Mathias Faust
%
%University:    NTU: Nanyang Technological University Singapore
%Filename:    	dec2binMF.m    
%Authors:     	Mathias Faust mfaust@mfnet.ch
%Supervisor:  	Assoc. Prof. Chang Chip Hong
%Creation:    	07.01.2009
%Last Change: 	-
%Description: 	creates binary array from x
%Input:       	x -> number or column vector
%Output:        y -> row vector or matrix with binary value (signed)
%Changes:		-
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [ y ] = dec2binMF( x )

h = max(ceil(log2(max(x)+1)), ceil(log2(abs(min(x)))))+1;

tmp = dec2bin(x,h);

y = (tmp == '1') | (tmp == '/');


