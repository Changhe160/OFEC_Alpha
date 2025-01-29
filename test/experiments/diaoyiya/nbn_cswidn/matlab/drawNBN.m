function drawNBN( readdir, savedir, filename)
    %nbn_filenpath =strcat(readdir,tspname);
    basicNodeSize = 15;
    numColor = 1000;
%    nbn_filenpath =strcat(readdir,filename);
%    nbn_filenpath=strcat(nbn_filenpath,"_nbn.txt");
    
    %nbn_filenpath= readdir+filename+ "_nbn.txt";
      %  nbn_mat  = readmatrix(nbn_filenpath);
      
      
    nbn_filenpath =strcat(readdir,filename);
    nbn_filenpath=strcat(nbn_filenpath,"_network.txt");    
        
    network_mat =readmatrix(nbn_filenpath, "NumHeaderLines",1);
    

        curoutfilename =strcat(savedir,filename);
    curoutfilename=strcat(curoutfilename,"_network.txt");    
    
%curoutfilename= savedir + filename + "_figure";


            
edge_a = network_mat(:,1)';
edge_b = network_mat(:,5)';
num_edge =size(edge_a,2);
for idx= 1:num_edge
    edge_a(idx) = edge_a(idx)+1;
    edge_b(idx) = edge_b (idx) + 1;
end
nbn_graph = graph(edge_a,edge_b);

mapColor = jet(numColor);
node_fit = network_mat(:,4);
node_ColorIdx = uint32(rescale(node_fit,1,numColor));
num_node =  size(node_fit,1);
node_color=  zeros(num_node,3);
        
for idx=1:num_node
    %node_color(idx,:) = mapColor(numColor-node_ColorIdx(idx)+1,:);
    node_color(idx,:) = mapColor(node_ColorIdx(idx),:);
end
node_size = zeros(num_node,1);
for idx=1:num_node
    node_size(idx)=basicNodeSize;
end
gray_color = [0.8 0.8 0.8];
black_color = [0 0 0];

%f = figure('visible','off');
clf;
nbn_plot = plot(nbn_graph,'XData',network_mat(:,2),'YData',network_mat(:,3),'ZData',network_mat(:,4),'NodeColor', gray_color , 'LineWidth',0.5 ,'EdgeColor',gray_color);
%nbn_plot.EdgeAlpha=0.2;
hold on;
nbn_points=  scatter3(network_mat(:,2),network_mat(:,3),network_mat(:,4),node_size,node_color,'filled');
%ExportThreeFigures();
%outputfilename = outputdir+ filename +"_fitness_nbn" ;





view(3);
setExportFigureType(curoutfilename,'origin',0.15);
view(2);
setExportFigureType(curoutfilename,'to',0.15);
view([180 0]);
setExportFigureType(curoutfilename,'view180',0.15);
view([30 30]);
setExportFigureType(curoutfilename,'view30_30',0.15);
view([90 30]);
setExportFigureType(curoutfilename,'view90_30',0.15);
view([120 30]);
setExportFigureType(curoutfilename,'view120_30',0.15);
end

