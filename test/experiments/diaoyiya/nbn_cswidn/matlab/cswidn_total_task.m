% 指定文件夹路径
folderPath = '//172.29.41.69/share/2018/diaoyiya/ofec-data/cwidn_total2/'; % 替换为你的文件夹路径
folderPath = '//172.29.41.69/share/2018/diaoyiya/ofec-data/cwidn_total3/'; % 替换为你的文件夹路径
% 获取文件夹下的所有文件信息
% 指定文件夹路径
%folderPath = 'your/folder/path'; % 替换为你的文件夹路径

% 获取文件夹下的所有文件信息
filesInfo = dir(fullfile(folderPath, '*_network.txt'));

% 初始化一个字典来存储唯一的文件名
uniqueFileNames = containers.Map();

for i = 1:length(filesInfo)
    if ~filesInfo(i).isdir  % 确保它是文件而不是文件夹
        % 读取文件名
        fileName = filesInfo(i).name;
        
        % 检查文件名是否包含'_'
        if contains(fileName, '_')
            % 找到最后一个'_'的位置
            indexes = strfind(fileName, '_');
            lastIndex = indexes(end);
            
            % 去掉最后一个'_'及其后面的所有字符
            modifiedFileName = fileName(1:lastIndex-1);
            
            % 检查修改后的文件名是否已经存在
            if ~uniqueFileNames.isKey(modifiedFileName)
                % 如果不存在，添加到字典中
                uniqueFileNames(modifiedFileName) = true;
            else
                % 如果存在，可以根据需要进行处理，例如不添加或跳过
                continue;
            end
        end
    end
end

% 将字典中的键转换为字符串数组
modifiedFileNames = uniqueFileNames.keys();

saveDir= "//172.29.203.176/e/DiaoYiya/experiment_cswidn/cswidn_total2/";
mkdir(saveDir);

for i = 1:length(modifiedFileNames)
% for i = 25:25
    fileName = modifiedFileNames{i};
    fprintf('Modified File Name %d: %s\n', i, fileName);
    drawNBN(folderPath, saveDir, fileName);

end

