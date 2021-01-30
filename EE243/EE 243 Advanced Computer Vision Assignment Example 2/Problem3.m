clear
clc

left_image = imread('viprectification_deskLeft.png');
right_image = imread('viprectification_deskRight.png');

% get SIFT features for left and right images
[framesL, descrsL] = getSIFTFeatures(left_image);
[framesR, descrsR] = getSIFTFeatures(right_image);
matched_Points = matchFeatures(descrsL',descrsR');

k = 5000; %repeat the process k times

error = [];
Fmatrix = [];
A = zeros(8,9);

for m = 1:k
    %pick 8 random points from matched points
    eight_points = randi([1 length(matched_Points)],1,8);
    total_distance = 0;
    for n = 1:length(eight_points)
        x1 = [framesL(1:2,eight_points(n));1];
        x2 = [framesR(1:2,eight_points(n));1];
        %compute matrix A
        A(n,:) = reshape(x2*x1',[1 9]);
    end
    %compute fundamental matrix F
    [~,~,V] = svd(A);
    F = (reshape(V(:,9),[3,3]))';
    [U,S,V] = svd(F);
    S(3,3) = 0;
    Fmatrix = [Fmatrix, U*S*V'];
    
    %compute Sampson distance for rest points
    total = 1:length(matched_Points);
    Lia = ismember(total,eight_points);
    mark = ones(1,length(matched_Points)) - Lia;
    for i = 1:length(matched_Points)
        if mark(i) ~= 0
            x1 = [framesL(1:2,i);1];
            x2 = [framesR(1:2,i);1];
            total_distance = total_distance + (x1'*F*x2)^2/(norm(F'*x1)^2+norm(F*x2)^2);
        end
    end
    Sampson_distance = total_distance./(length(matched_Points)-length(eight_points));
    error = [error,Sampson_distance];
end

%print out the mean error and the fundamental matrix as needed
mean_error = mean(error)

[~,index] = find(error==min(error));
Fmatrix_least_error = Fmatrix(1:3, 3*index-2:3*index)




