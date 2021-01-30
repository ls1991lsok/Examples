clear
clc

%I take image pair 1 & 9 
image1 = imread('images/Image1.tif');
[imagePoints1, boardSize1, imagesUsed1] = detectCheckerboardPoints(image1);

image2 = imread('images/Image9.tif');
[imagePoints9, boardSize9, imagesUsed2] = detectCheckerboardPoints(image2);

xi = imagePoints1(:, 1);
yi = imagePoints1(:, 2);
xi_pi = imagePoints9(:, 1);
yi_pi = imagePoints9(:, 2);
A = zeros(length(xi(:))*2,9); % A = 2n * 9

%from lecture slides page 19-20
for i = 1:length(xi(:))
    p = [xi(i),yi(i),1];
    zero = [0 0 0];
    p_pi = [xi_pi(i);yi_pi(i)];
    right_side = -(p_pi*p);
    A((i-1)*2+1:(i-1)*2+2,1:9) = [[p, zero;zero, p], right_side];
end

[~, ~, V] = svd(A, 'econ');
H = (reshape(V(:,9), 3, 3)).' %print out the homography matrix as needed

%follow the instructions of detectCheckerboardPoints from Matlab help
%center
figure;
image1 = image1(imagesUsed1);
for i = 1:numel(image1)
  I1 = imread('images/Image1.tif');
  subplot(1, 2, 1);
  imshow(I1);
  hold on;
  plot(imagePoints1(:,1,i),imagePoints1(:,2,i),'r*');
  title('Image1');
  
  I2 = image2;
  subplot(1, 2, 2);
  imshow(I2);
  hold on;
  plot(imagePoints9(:,1,i),imagePoints9(:,2,i),'r*');
  title('Image9');
end

figure;
showMatchedFeatures(imread('images/Image1.tif'),image2,imagePoints1,imagePoints9,'montage', 'PlotOptions', {'r*', 'g*', 'y-'});
title('Image 1 corners matched with Image 9 by one-to-one');

