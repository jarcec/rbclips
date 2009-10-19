require 'test/unit'
require 'rbclips'

class Test_Fact < Test::Unit::TestCase
  def test_exists
    assert Clips.constants.member?(:Fact)
  end

  def get_animal
    Clips::Template.new 'animal' do |t| 
      t.slot :name
      t.slot :age
      t.slot :race
    end
  end

  def test_ordered
    assert_nothing_raised   { Clips::Fact.new 'human', [:age, 'name', 30, 1.5] }
    assert_nothing_raised   { Clips::Fact.new 'human', %w(jedna dva tri ctyri) }

    a = Clips::Fact.new 'human', [:jarcec, 22]
    assert_equal a.instance_eval { @slots }, [:jarcec, 22]
    assert_equal a.instance_eval { @template }, 'human'

    assert a.save
    assert !a.save
    assert a.destroy!
    assert !a.destroy!

    assert a.ordered?
  end

  def test_nonordered
    template = get_animal

    assert_raise(Clips::ArgumentError)  { Clips::Fact.new template, :name  => 'agatha' }
    assert template.save
    assert_nothing_raised               { Clips::Fact.new template, :name  => 'agatha' }
    assert_nothing_raised               { Clips::Fact.new template, 'name' => 'agatha' }
    assert_raise(Clips::ArgumentError)  { Clips::Fact.new template, :years => 'agatha' }
    assert_raise(Clips::ArgumentError)  { Clips::Fact.new template, 29     => 'agatha' }

    a = Clips::Fact.new template, :name => 'agatha', 'age' => 30
    assert_equal a.instance_eval { @slots }, { :name => 'agatha', :age => 30}
    assert_equal a.instance_eval { @template }, template

    assert !a.ordered?
    assert template.destroy!
  end

  def test_to_s
    c = Clips::Fact.new 'human', %w(jarek 30)
    assert_equal c.to_s, '(human "jarek" "30")'

    template = get_animal
    assert template.save
    
    c = Clips::Fact.new template, :name => 'ales', :age => 30, 'race' => 'pes'
    assert_equal c.to_s, '(animal (name "ales") (age 30) (race "pes"))'

    c = Clips::Fact.new template, :name => 'ales', :race => 'pes'
    assert_equal c.to_s, '(animal (name "ales") (race "pes"))'

    # Array ~= multifield
    c = Clips::Fact.new template, :name => 'ales', :race => %w(pes dog)
    assert_equal c.to_s, '(animal (name "ales") (race "pes" "dog"))'

    assert template.destroy!
  end

  def test_clone
    a = Clips::Fact.new 'human', %w(x y z)
    b = a.clone
    c = b.dup

    assert a.eql? b
    assert b.equal? c
    assert a == c

    a.save
    assert ! ( a.eql? b )
    assert ! ( a.equal? b )
    assert ! ( a == b )
    assert b == c
  end

  def test_slots
    a = Clips::Fact.new 'human', %w(x y z)
    assert a.slots
    assert_equal a.slots, ['x', 'y', 'z']

    a = Clips::Fact.new 'human', [30, :ahoj, 'zdar', 4.2]
    assert_equal a.slots, [30, :ahoj, 'zdar', 4.2]
  end

  def test_slot
    template = get_animal
    assert template.save

    a = Clips::Fact.new template, :name => "Bruno", :age => 30
    assert_equal a.slot(:name), "Bruno"
    assert_equal a.slot(:age), 30
    assert_equal a.slot(:race), nil
    assert_raise(Clips::ArgumentError) { a.slot(:bulb) }
    assert_raise(Clips::ArgumentError) { a.slot(30) }
    assert_raise(Clips::ArgumentError) { a.slot(2.4) }

    assert template.destroy!
  end

  def test_name
    # Ordered variant
    a = Clips::Fact.new "human", %w(jedna dve tri)
    assert_equal a.name, "human"
    
    # Nonordered variant
    template = get_animal
    assert template.save

    a = Clips::Fact.new template, :name => "Bruno", :age => 30
    assert_equal a.template, template

    assert template.destroy!
  end

end
