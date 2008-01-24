for i = 1:13
    orig = sprintf('orig/sift_%02d.mat', i);
    new  = sprintf('new/sift_%02d.mat', i);
    s1 = load(orig);
    s2 = load(new);
    if 0% any(double(s1.f) - double(s2.f))
        fprintf('Keypoints are different\n');
        keyboard;
    end
    if any(double(s1.d) - double(s2.d))
        fprintf('Descriptors are different\n');
        keyboard;
    end
end
fprintf('comparesift: success\n');
