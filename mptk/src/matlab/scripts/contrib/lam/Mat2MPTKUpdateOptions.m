function Struct2 = MAT2MPTKUpdateOptions(Struct1, Struct2)

if ~isempty(Struct1)
fn1 = fieldnames(Struct1);


for k1 = 1:size(fn1, 1)

    Struct2.(fn1{k1}) = Struct1.(fn1{k1});
end
end
