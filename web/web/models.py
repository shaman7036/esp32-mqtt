from django.db import models

class BaseModel(models.Model):
    created_at = models.DateTimeField(
        auto_now_add=True,
    )
    
    class Meta:
        abstract = True

class Temperature(BaseModel):
    value = models.DecimalField(
        blank=False,
        decimal_places=2,
        max_digits=5,
    )

class Humidity(BaseModel):
    value = models.DecimalField(
        blank=False,
        decimal_places=2,
        max_digits=5,
    )

class Pressure(BaseModel):
    value = models.DecimalField(
        blank=False,
        decimal_places=2,
        max_digits=6,
    )

class Co2(BaseModel):
    value = models.IntegerField(
        blank=False,
    )

class Tvoc(BaseModel):
    value = models.IntegerField(
        blank=False,
    )
