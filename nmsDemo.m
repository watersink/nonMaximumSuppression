boxes=[200,200,400,400,0.99;
        220,220,420,420,0.9;
        100,100,150,150,0.82;
        200,240,400,440,0.5];
overlap=0.8;
pick = nms(boxes, overlap);
figure;
for (i=1:size(boxes,1))
    rectangle('Position',[boxes(i,1),boxes(i,2),boxes(i,3)-boxes(i,1),boxes(i,4)-boxes(i,2)],'EdgeColor','y','LineWidth',6);
    text(boxes(i,1),boxes(i,2),num2str(boxes(i,5)),'FontSize',14,'color','b');
end
for (i=1:size(pick,1))
    rectangle('Position',[boxes(pick(i),1),boxes(pick(i),2),boxes(pick(i),3)-boxes(pick(i),1),boxes(pick(i),4)-boxes(pick(i),2)],'EdgeColor','r','LineWidth',2);
end
axis([0 600 0 600]);