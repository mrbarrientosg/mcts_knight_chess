# Lucy Knight Controller

Controlador basado en el metodo de monte carlo tree search.

El controlador tiene una variante de paralelización en el nodo raiz. 

## Compilación y ejecución

Para compilar el codigo debe ejecutar el siguiente comando de linea:
```bash
$ cd knight_chess
$ g++ -O3 *.cpp -o lucy_controller --std=c++11 -lpthread
```

El video explicativo se puede ver en este [link](https://drive.google.com/file/d/1lDSMDAbuG7eXynWXwN_4Cmh0jTMn5n9C/view?usp=sharing)

La coevaluacion [aqui](COEVALUACION.md)

El documento de la descripcion del algoritmo [aqui](Descripcion_del_algoritmo.pdf)
