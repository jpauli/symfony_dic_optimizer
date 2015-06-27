Symfony Dependency Injection Container Optimizer
================================================

This extension declares a Symfony\Component\DependencyInjection\ContainerOptimizerInterface interface.

Simply make your container implement it :

```php
<?php
use Symfony\Component\DependencyInjection\ContainerOptimizerInterface;
use Symfony\Component\DependencyInjection\Container;

class appDevDebugProjectContainer extends Container implements ContainerOptimizerInterface
{

}
```
The interface will replace the get() method by a C implemented one, more performant hopefully.

Info:
====

	> phpize
	> ./configure
	> make
	> make install
	

* Supports PHP 5.3, 5.4, 5.5 and 5.6
* Supports ZTS