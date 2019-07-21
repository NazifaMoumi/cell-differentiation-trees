import numpy as np
import pandas as pd
from sklearn.decomposition import PCA
import matplotlib.pyplot as plt

dat = np.loadtxt( 'overlap_datarepresentation_2.txt' )
print(len(dat))
print(len(dat[0]))
print(dat[0,7].astype( np.int ))

pca = PCA(n_components=2)

principalComponents = pca.fit_transform(dat)

principalDf = pd.DataFrame(data = principalComponents
             , columns = ['principal component 1', 'principal component 2'])

print(len(principalComponents))
print(len(principalComponents[0]))
print(principalComponents)

x = principalComponents[[4,5,6,7,16],0]
y = principalComponents[[4,5,6,7,16],1]
plt.scatter(x,y,label='hESC',color='red')
x = principalComponents[[2,3,13],0]
y = principalComponents[[2,3,13],1]
plt.scatter(x,y,label='blood',color='blue')
x = principalComponents[[9],0]
y = principalComponents[[9],1]
plt.scatter(x,y,label = 'endothelial',color='yellow')
x = principalComponents[[0],0]
y = principalComponents[[0],1]
plt.scatter(x,y,label = 'fibroblast',color='purple')
x = principalComponents[[1,8,10,11,12,14,15],0]
y = principalComponents[[1,8,10,11,12,14,15],1]
plt.scatter(x,y,label='epithelial',color='lime')

plt.legend(loc = 'upper right')
plt.xlabel("PC1 (variance = %.2f%%)" %(pca.explained_variance_ratio_[0]*100))
plt.ylabel("PC2 (variance = %.2f%%)" %(pca.explained_variance_ratio_[1]*100))

f=open('file_sequence.txt',"r")
lines=f.readlines()
annot_list=[]
for x in lines:
    annot_list.append(x.split('	')[1])
f.close()

for i in range(0,17):
    plt.annotate(annot_list[i], (principalComponents[i][0], principalComponents[i][1]))


plt.show()






