% 获取指定文件夹路径
folderPath = '\\172.24.178.59\share\2018\diaoyiya\ofec-data\paper_con_realworld\AAA_algOneTrait\CEC2022T_F01\'; % 将这里替换为实际的文件夹路径
%folderPath = "\\172.24.34.11\share\2018\diaoyiya\ofec-data\paper_con_realworld\NBN_trait_network3\";
outputdir ='\\172.24.6.26\f\DiaoYiya\paper_con_realworld\AlgOneTraitNBN/CEC2022T_F01\';
mkdir(outputdir);

% 获取文件夹下所有文件名
fileNames = dir(fullfile(folderPath, '*_network.txt'));

% 遍历文件名数组
for i = 1:length(fileNames)
%i=3;
    % 获取完整文件路径
    filePath = fullfile(folderPath, fileNames(i).name);
    display(filePath);
    display(fileNames(i).name);
    curfilename = fileNames(i).name(1:end-length('_network.txt'));
    display(curfilename);
    
    drawNBN(folderPath, , curfilename);

    % 读取文件内容，可以根据文件的具体格式选择合适的读取函数
%    data = importdata(filePath);
    % 在这里可以对读取的数据进行处理
end