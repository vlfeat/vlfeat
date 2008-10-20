function vl_test_imintegral

I = ones(5,6);

correct = [1     2     3     4     5     6;
           2     4     6     8    10    12;
           3     6     9    12    15    18;
           4     8    12    16    20    24;
           5    10    15    20    25    30;];

if ~all(all(slow_imintegral(I) == correct))
    fprintf('test_imintegral: FAIL slow ones test\n');
    keyboard;
end

if ~all(all(vl_imintegral(I) == correct))
    fprintf('test_imintegral: FAIL ones test\n');
    keyboard;
end

I = repmat(ones(5,6), [1 1 3]);
integral = vl_imintegral(I);
if ~all(all(all(integral == repmat(correct,[1 1 3]))))
    fprintf('test_imintegral: FAIL multidimensional ones test\n');
    keyboard;
end

ntest = 50;
for i = 1:ntest
    I = rand(5);
    integral = vl_imintegral(I);
    slow_integral = slow_imintegral(I);
    err = abs(integral - slow_integral);
    if max(err(:)) > 0.00001
        fprintf('test_imintegral: FAIL random test\n');
        keyboard;
    end
end

fprintf('test_imintegral: passed.\n');

% The slow but obvious way
function integral = slow_imintegral(I)
integral = zeros(size(I));
for k = 1:size(I,3)
    for r = 1:size(I,1)
        for c = 1:size(I,2)
            integral(r,c,k) = sum(sum(I(1:r,1:c,k)));
        end
    end
end

