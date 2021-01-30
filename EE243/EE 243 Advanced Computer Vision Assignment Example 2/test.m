clear
clc

left_image = imread('viprectification_deskLeft.png');
right_image = imread('viprectification_deskRight.png');

% get SIFT features and match
[framesL, descrsL] = getSIFTFeatures(left_image);
[framesR, descrsR] = getSIFTFeatures(right_image);
matched_Points = matchFeatures(descrsL',descrsR');
coordL = framesL(1:2, matched_Points(:, 1))';
coordR = framesR(1:2, matched_Points(:, 2))';

fNorm8Point = estimateFundamentalMatrix(coordL,coordR,'Method','Norm8Point')