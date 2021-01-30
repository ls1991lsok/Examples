function [point3D] = get3DPoints(points2D)

A(1,:) = points2D(1,:);
A(2,:) = points2D(3,:);
A(3,:) = points2D(2,:);
A(4,:) = points2D(4,:);

A1 = A - mean(points2D')';

[~,S,V] = svd(A1);

D1 = S(1:3,1:3);
V1 = V(1:3,:);

Points = sqrt(D1)*V1;
point3D = Points;
end

